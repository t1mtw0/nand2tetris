#include "parser.h"

#include <ctype.h>
#include <string.h>

#include "common.h"

struct Parser new_parser(char *proj_dir, char *fi) {
    int proj_dir_l = strlen(proj_dir);
    int fi_l = strlen(fi);
    char fn[proj_dir_l + fi_l + 2];
    strncpy(fn, proj_dir, proj_dir_l);
    strncpy(fn + proj_dir_l, "/", 1);
    strncpy(fn + proj_dir_l + 1, fi, fi_l + 1);
    FILE *fp = fopen(fn, "r");
    if (!fp)
        exit(1);
    struct Parser p = {.fi = fi,
                       .fp = fp,
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
    // remove potential comments from line
    char *line = l;
    char *ss = strchr(line, '/');
    if (ss && *(ss + 1) == '/') {
        *ss = '\0';
    }
    // trim whitespace
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
    // skip lines that are empty after processing
    if (line[0] == '\0') {
        free(l);
        return advance(p);
    }
    // free existing arg1 and arg2
    if (!p->arg1) {
        free(p->arg1);
        p->arg1 = NULL;
    }
    if (!p->arg2) {
        free(p->arg2);
        p->arg2 = NULL;
    }
    // handle two argument commands
    if (!strncmp(line, "push", 4) || !strncmp(line, "pop", 3) ||
        !strncmp(line, "function", 8) || !strncmp(line, "call", 4)) {
        // arg1
        char *ss = NULL;
        if (!strncmp(line, "push", 4) || !strncmp(line, "call", 4))
            ss = line + 4;
        else if (!strncmp(line, "pop", 3))
            ss = line + 3;
        else if (!strncmp(line, "function", 8))
            ss = line + 8;
        while (*ss != '\0') {
            if (!isspace((unsigned char)*ss))
                break;
            ss++;
        }
        if (*ss == '\0') {
            free(l);
            fprintf(stderr, "%s\n", "Expected segment after command.");
            exit(1);
        }
        char *es = ss;
        while (*es != '\0') {
            if (isspace((unsigned char)*es))
                break;
            es++;
        }
        if (*es == '\0') {
            free(l);
            fprintf(stderr, "%s\n", "Expected index after segment.");
            exit(1);
        }
        char *arg1 = malloc(sizeof(char) * ((int)(es - ss) + 1));
        strncpy(arg1, ss, (int)(es - ss));
        arg1[es - ss] = '\0';
        p->arg1 = arg1;
        // arg2
        char *si = es;
        while (*si != '\0') {
            if (!isspace((unsigned char)*si))
                break;
            si++;
        }
        if (*si == '\0') {
            free(l);
            fprintf(stderr, "%s\n", "Expected index after segment.");
            exit(1);
        }
        char *ei = si;
        while (*ei != '\0') {
            if (isspace((unsigned char)*ei))
                break;
            ei++;
        }
        char *arg2 = malloc(sizeof(char) * ((int)(ei - si) + 1));
        strncpy(arg2, si, (int)(ei - si));
        arg2[ei - si] = '\0';
        p->arg2 = arg2;
    }
    // handle single arg commands
    if (!strncmp(line, "label", 5) || !strncmp(line, "goto", 4) || !strncmp(line, "if-goto", 7)) {
        char *ss = NULL;
        if (!strncmp(line, "label", 5))
            ss = line + 5;
        else if (!strncmp(line, "goto", 4))
            ss = line + 4;
        else if (!strncmp(line, "if-goto", 7))
            ss = line + 7;
        while (*ss != '\0') {
            if (!isspace((unsigned char)*ss))
                break;
            ss++;
        }
        if (*ss == '\0') {
            free(l);
            fprintf(stderr, "%s\n", "Expected segment after command.");
            exit(1);
        }
        char *es = ss;
        while (*es != '\0') {
            if (isspace((unsigned char)*es))
                break;
            es++;
        }
        char *arg1 = malloc(sizeof(char) * ((int)(es - ss) + 1));
        strncpy(arg1, ss, (es - ss));
        arg1[es - ss] = '\0';
        p->arg1 = arg1;

    }
    // parses according to command
    if (!strncmp(line, "push", 4)) {
        p->command_type = C_PUSH;
    } else if (!strncmp(line, "pop", 3)) {
        p->command_type = C_POP;
    } else if (!strncmp(line, "add", 3)) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 4);
        strncpy(arg1, "add", 4);
        p->arg1 = arg1;
    } else if (!strncmp(line, "sub", 3)) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 4);
        strncpy(arg1, "sub", 4);
        p->arg1 = arg1;
    } else if (!strncmp(line, "neg", 3)) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 4);
        strncpy(arg1, "neg", 4);
        p->arg1 = arg1;
    } else if (!strncmp(line, "eq", 2)) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 3);
        strncpy(arg1, "eq", 3);
        p->arg1 = arg1;
    } else if (!strncmp(line, "gt", 2)) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 3);
        strncpy(arg1, "gt", 3);
        p->arg1 = arg1;
    } else if (!strncmp(line, "lt", 2)) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 3);
        strncpy(arg1, "lt", 3);
        p->arg1 = arg1;
    } else if (!strncmp(line, "and", 3)) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 4);
        strncpy(arg1, "and", 4);
        p->arg1 = arg1;
    } else if (!strncmp(line, "or", 2)) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 3);
        strncpy(arg1, "or", 3);
        p->arg1 = arg1;
    } else if (!strncmp(line, "not", 3)) {
        p->command_type = C_ARITHMETIC;
        char *arg1 = malloc(sizeof(char) * 4);
        strncpy(arg1, "not", 4);
        p->arg1 = arg1;
    } else if (!strncmp(line, "label", 5)) {
        p->command_type = C_LABEL;
    } else if (!strncmp(line, "goto", 4)) {
        p->command_type = C_GOTO;
    } else if (!strncmp(line, "if-goto", 7)) {
        p->command_type = C_IF;
    } else if (!strncmp(line, "function", 8)) {
        p->command_type = C_FUNCTION;
    } else if (!strncmp(line, "call", 4)) {
        p->command_type = C_CALL;
    } else if (!strncmp(line, "return", 6)) {
        p->command_type = C_RETURN;
        char *arg1 = malloc(sizeof(char) * 7);
        strncpy(arg1, "return", 7);
        p->arg1 = arg1;
    } else {
        fprintf(stderr, "%s\n", "Unrecognized VM Command.");
        free(l);
        exit(1);
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

void end_parser(struct Parser *p) {
    fclose(p->fp);
    if (p->arg1)
        free(p->arg1);
    if (p->arg2)
        free(p->arg2);
}
