#include "table.h"
#include "common.h"
#include <string.h>

#define MAX_LOAD 0.75

static int hash(char *key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

void print_table(struct Table *t) {
    for (int i = 0; i < t->capacity; i++) {
        struct Entry *entry = &t->entries[i];
        if (!entry->name)
            continue;
        printf("Name: %s\n", entry->name);
        printf("Type: %s\n", entry->value.type);
        switch (entry->value.kind) {
        case KD_ARG:
            printf("Kind: KD_ARG\n");
            break;
        case KD_FIELD:
            printf("Kind: KD_FIELD\n");
            break;
        case KD_STATIC:
            printf("Kind: KD_STATIC\n");
            break;
        case KD_VAR:
            printf("Kind: KD_VAR\n");
            break;
        case KD_NONE:
            break;
        }
        printf("Index: %d\n", entry->value.index);
        printf("\n");
    }
}

static struct Entry *find_entry(struct Entry *entries, int capacity,
                                char *name) {
    int hashed = hash(name, strlen(name)) & (capacity - 1);
    struct Entry *tombstone = NULL;

    for (;;) {
        struct Entry *entry = &entries[hashed];
        if (entry->name == NULL) {
            if (entry->value.type == KD_NONE) {
                return tombstone != NULL ? tombstone : entry;
            } else {
                if (tombstone == NULL)
                    tombstone = entry;
            }
        } else if (!strcmp(entry->name, name)) {
            return entry;
        }
        hashed = (hashed + 1) & (capacity - 1);
    }
}

static void free_table(struct Table *t) {
    for (int i = 0; i < t->capacity; i++) {
        struct Entry *entry = &t->entries[i];
        if (entry->name) {
            free(entry->name);
            entry->name = NULL;
        }
        if (entry->value.type) {
            free(entry->value.type);
            entry->value.type = NULL;
        }
    }
    free(t->entries);
}

static void adjust_capacity(struct Table *t, int new_capacity) {
    struct Entry *entries = malloc(sizeof(struct Entry) * new_capacity);
    for (int i = 0; i < new_capacity; i++) {
        entries[i].name = NULL;
        entries[i].value =
            (struct Value){.type = NULL, .kind = KD_NONE, .index = 0};
    }
    t->count = 0;

    for (int i = 0; i < t->capacity; i++) {
        struct Entry *entry = &t->entries[i];
        if (!entry->name)
            continue;
        struct Entry *dest = find_entry(entries, new_capacity, entry->name);
        dest->name = entry->name;
        dest->value = entry->value;
        t->count++;
    }

    free(t->entries);
    t->entries = entries;
    t->capacity = new_capacity;
}

static int update_var_count(struct Table *t, enum Kinds k) {
    switch (k) {
    case KD_ARG:
        return t->n_arg++;
    case KD_FIELD:
        return t->n_field++;
    case KD_STATIC:
        return t->n_static++;
    case KD_VAR:
        return t->n_var++;
    case KD_NONE:
        exit(1);
    }
}

struct Table new_table() {
    struct Table t = {.entries = NULL,
                      .count = 0,
                      .capacity = 0,
                      .n_static = 0,
                      .n_field = 0,
                      .n_arg = 0,
                      .n_var = 0};
    return t;
}

void end_table(struct Table *t) { free_table(t); }

void reset(struct Table *t) {
    free_table(t);
    t->entries = NULL;
    t->capacity = 0;
    t->count = 0;
    t->n_arg = 0;
    t->n_field = 0;
    t->n_static = 0;
    t->n_var = 0;
}

void define(struct Table *t, char *name, char *type, enum Kinds k) {
    if (t->count + 1 > (int)(t->capacity * MAX_LOAD)) {
        int new_capacity = t->capacity < 8 ? 8 : t->capacity * 2;
        adjust_capacity(t, new_capacity);
    }

    struct Entry *entry = find_entry(t->entries, t->capacity, name);
    bool is_new_entry = entry->name == NULL && entry->value.type == KD_NONE;
    if (is_new_entry)
        t->count++;
    if (entry->name == NULL || entry->value.kind != k)
        entry->value.index = update_var_count(t, k);
    entry->name = name;
    entry->value.type = type;
    entry->value.kind = k;
}

int var_count(struct Table *t, enum Kinds k) {
    switch (k) {
    case KD_ARG:
        return t->n_arg;
    case KD_FIELD:
        return t->n_field;
    case KD_STATIC:
        return t->n_static;
    case KD_VAR:
        return t->n_var;
    case KD_NONE:
        exit(1);
    }
}

enum Kinds kind_of(struct Table *t, char *name) {
    if (t->count == 0)
        return KD_NONE;
    struct Entry *entry = find_entry(t->entries, t->capacity, name);
    if (!entry->name)
        return KD_NONE;
    return entry->value.kind;
}

char *type_of(struct Table *t, char *name) {
    if (t->count == 0)
        return NULL;
    struct Entry *entry = find_entry(t->entries, t->capacity, name);
    if (!entry->name)
        return NULL;
    return entry->value.type;
}

int index_of(struct Table *t, char *name) {
    if (t->count == 0)
        return -1;
    struct Entry *entry = find_entry(t->entries, t->capacity, name);
    if (!entry->name)
        return -1;
    return entry->value.index;
}
