#ifndef TABLE_H
#define TABLE_H

#include "common.h"

enum Kinds {
    KD_NONE,
    KD_STATIC,
    KD_FIELD,
    KD_ARG,
    KD_VAR,
};

struct Value {
    char *type;
    enum Kinds kind;
    int index;
};

struct Entry {
    char *name;
    struct Value value;
};

struct Table {
    struct Entry *entries;
    int count;
    int capacity;
    int n_static;
    int n_field;
    int n_arg;
    int n_var;
};

struct Table new_table();
void end_table(struct Table *t);
void print_table(struct Table *t);
void reset(struct Table *t);
void define(struct Table *t, char *name, char *type, enum Kinds k);
int var_count(struct Table *t, enum Kinds k);
enum Kinds kind_of(struct Table *t, char *name);
char *type_of(struct Table *t, char *name);
int index_of(struct Table *t, char *name);

#endif
