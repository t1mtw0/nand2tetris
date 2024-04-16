#include "vm_writer.h"
#include "common.h"

void write_push(FILE *fp, enum Segments seg, int index) {
    char s[50];
    switch (seg) {
    case SEG_CONSTANT:
        snprintf(s, 50, "push constant %d\n", index);
        fputs(s, fp);
        break;
    case SEG_ARGUMENT:
        snprintf(s, 50, "push argument %d\n", index);
        fputs(s, fp);
        break;
    case SEG_LOCAL:
        snprintf(s, 50, "push local %d\n", index);
        fputs(s, fp);
        break;
    case SEG_POINTER:
        snprintf(s, 50, "push pointer %d\n", index);
        fputs(s, fp);
        break;
    case SEG_STATIC:
        snprintf(s, 50, "push static %d\n", index);
        fputs(s, fp);
        break;
    case SEG_TEMP:
        snprintf(s, 50, "push temp %d\n", index);
        fputs(s, fp);
        break;
    case SEG_THIS:
        snprintf(s, 50, "push this %d\n", index);
        fputs(s, fp);
        break;
    case SEG_THAT:
        snprintf(s, 50, "push that %d\n", index);
        fputs(s, fp);
        break;
    }
}

void write_pop(FILE *fp, enum Segments seg, int index) {
    char s[50];
    switch (seg) {
    case SEG_CONSTANT:
        snprintf(s, 50, "pop constant %d\n", index);
        fputs(s, fp);
        break;
    case SEG_ARGUMENT:
        snprintf(s, 50, "pop argument %d\n", index);
        fputs(s, fp);
        break;
    case SEG_LOCAL:
        snprintf(s, 50, "pop local %d\n", index);
        fputs(s, fp);
        break;
    case SEG_POINTER:
        snprintf(s, 50, "pop pointer %d\n", index);
        fputs(s, fp);
        break;
    case SEG_STATIC:
        snprintf(s, 50, "pop static %d\n", index);
        fputs(s, fp);
        break;
    case SEG_TEMP:
        snprintf(s, 50, "pop temp %d\n", index);
        fputs(s, fp);
        break;
    case SEG_THIS:
        snprintf(s, 50, "pop this %d\n", index);
        fputs(s, fp);
        break;
    case SEG_THAT:
        snprintf(s, 50, "pop that %d\n", index);
        fputs(s, fp);
        break;
    }
}

void write_arithmetic(FILE *fp, enum AriCommands c) {
    switch (c) {
    case A_ADD:
        fputs("add\n", fp);
        break;
    case A_SUB:
        fputs("sub\n", fp);
        break;
    case A_GT:
        fputs("gt\n", fp);
        break;
    case A_LT:
        fputs("lt\n", fp);
        break;
    case A_AND:
        fputs("and\n", fp);
        break;
    case A_OR:
        fputs("or\n", fp);
        break;
    case A_EQ:
        fputs("eq\n", fp);
        break;
    case A_NEG:
        fputs("neg\n", fp);
        break;
    case A_NOT:
        fputs("not\n", fp);
        break;
    }
}

void write_label(FILE *fp, char *label) {
    char s[50];
    snprintf(s, 50, "label %s\n", label);
    fputs(s, fp);
}

void write_goto(FILE *fp, char *label) {
    char s[50];
    snprintf(s, 50, "goto %s\n", label);
    fputs(s, fp);
}

void write_if(FILE *fp, char *label) {
    char s[50];
    snprintf(s, 50, "if-goto %s\n", label);
    fputs(s, fp);
}

void write_call(FILE *fp, char *name, int n_args) {
    char s[100];
    snprintf(s, 100, "call %s %d\n", name, n_args);
    fputs(s, fp);
}

void write_function(FILE *fp, char *name, int n_vars) {
    char s[100];
    snprintf(s, 100, "function %s %d\n", name, n_vars);
    fputs(s, fp);
}

void write_return(FILE *fp) { fputs("return\n", fp); }
