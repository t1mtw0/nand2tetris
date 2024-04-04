#include "parser.h"

#include <ctype.h>
#include <string.h>

#include "common.h"

struct Parser new_parser(FILE *fp) {
    struct Parser p = {.fp = fp,
                       .has_more_lines = true,
                       .command_type = C_NULL,
                       .arg1 = NULL,
                       .arg2 = NULL};
    return p;
}

bool advance(struct Parser *p) {
    // check if no more lines
    if (!p->has_more_lines)
        return false;
    // get line from file
    char *l = NULL;
    size_t len = 0;
    size_t read = getline(&l, &len, p->fp);
    if (read == -1) {
        free(l);
        p->has_more_lines = false;
        return false;
    }
    // trim whitespace
    char *line = l;
    while (isspace((unsigned char)*line)) {
        line++;
        read--;
    }
    char *end = line + read - 1;
    while (end > line && isspace((unsigned char)*end)) {
        end--;
        read--;
    }
    end[1] = '\0';
    // skip comments and newlines
    if (line[0] == '/' && line[1] == '/') {
        free(l);
        return advance(p);
    }
    if (line[0] == '\0') {
        free(l);
        return advance(p);
    }
    // free existing arg1 and arg2
    if (p->arg1 != NULL) {
        free(p->arg1);
        p->arg1 = NULL;
    }
    if (p->arg2 != NULL) {
        free(p->arg2);
        p->arg2 = NULL;
    }
    // parses according to command
    if (strncmp(line, "push", 4) == 0) {
        p->command_type = C_PUSH;
        char *l = line + 5;
        while (*l != '\0') {
            if (isspace((unsigned char)*l))
                break;
            l++;
        }
        if (*l == '\0')
            exit(1);
        char *arg1 = malloc(sizeof(char) * (l - (line + 5)));
        strncpy(arg1, line + 5, l - (line + 5));
        p->arg1 = arg1;
        l++;
        char *m = l;
        while (*m != '\0') {
            m++;
        }
        char *arg2 = malloc(sizeof(char) * (m - l));
        strncpy(arg2, l, m - l);
        p->arg2 = arg2;
    } else if (strncmp(line, "pop", 3) == 0) {
        p->command_type = C_POP;
        char *l = line + 4;
        while (*l != '\0') {
            if (isspace((unsigned char)*l))
                break;
            l++;
        }
        if (*l == '\0')
            exit(1);
        char *arg1 = malloc(sizeof(char) * (l - (line + 4)));
        strncpy(arg1, line + 4, l - (line + 4));
        p->arg1 = arg1;
        l++;
        char *m = l;
        while (*m != '\0') {
            m++;
        }
        char *arg2 = malloc(sizeof(char) * (m - l));
        strncpy(arg2, l, m - l);
        p->arg2 = arg2;
    } else if (strncmp(line, "add", 3) == 0) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 3);
        strncpy(arg1, "add", 3);
        p->arg1 = arg1;
        p->arg2 = NULL;
    } else if (strncmp(line, "sub", 3) == 0) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 3);
        strncpy(arg1, "sub", 3);
        p->arg1 = arg1;
        p->arg2 = NULL;
    } else if (strncmp(line, "neg", 3) == 0) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 3);
        strncpy(arg1, "neg", 3);
        p->arg1 = arg1;
        p->arg2 = NULL;
    } else if (strncmp(line, "eq", 2) == 0) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 2);
        strncpy(arg1, "eq", 2);
        p->arg1 = arg1;
        p->arg2 = NULL;
    } else if (strncmp(line, "gt", 2) == 0) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 2);
        strncpy(arg1, "gt", 2);
        p->arg1 = arg1;
        p->arg2 = NULL;
    } else if (strncmp(line, "lt", 2) == 0) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 2);
        strncpy(arg1, "lt", 2);
        p->arg1 = arg1;
        p->arg2 = NULL;
    } else if (strncmp(line, "and", 3) == 0) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 3);
        strncpy(arg1, "and", 3);
        p->arg1 = arg1;
        p->arg2 = NULL;
    } else if (strncmp(line, "or", 2) == 0) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 2);
        strncpy(arg1, "or", 2);
        p->arg1 = arg1;
        p->arg2 = NULL;
    } else if (strncmp(line, "not", 3) == 0) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 3);
        strncpy(arg1, "not", 3);
        p->arg1 = arg1;
        p->arg2 = NULL;
    }
    free(l);
    return true;
}

enum CommandType command_type(struct Parser *p) { return p->command_type; }

char *arg1(struct Parser *p) {
    if (p->command_type == C_RETURN)
        return NULL;
    return p->arg1;
}

char *arg2(struct Parser *p) {
    if (p->command_type != C_PUSH && p->command_type != C_POP &&
        p->command_type != C_FUNCTION && p->command_type != C_CALL)
        return NULL;
    return p->arg2;
}

void free_parser(struct Parser *p) {
    if (p->arg1 != NULL)
        free(p->arg1);
    if (p->arg2 != NULL)
        free(p->arg2);
}
