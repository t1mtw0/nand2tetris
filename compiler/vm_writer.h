#ifndef VM_WRITER_H
#define VM_WRITER_H

#include "common.h"

enum Segments {
    SEG_CONSTANT,
    SEG_ARGUMENT,
    SEG_LOCAL,
    SEG_STATIC,
    SEG_THIS,
    SEG_THAT,
    SEG_POINTER,
    SEG_TEMP,
};

enum AriCommands {
    A_ADD,
    A_SUB,
    A_NEG,
    A_EQ,
    A_GT,
    A_LT,
    A_AND,
    A_OR,
    A_NOT,
};

void write_push(FILE *fp, enum Segments seg, int index);
void write_pop(FILE *fp, enum Segments seg, int index);
void write_arithmetic(FILE *fp, enum AriCommands c);
void write_label(FILE *fp, char *label);
void write_goto(FILE *fp, char *label);
void write_if(FILE *fp, char *label);
void write_call(FILE *fp, char *name, int n_args);
void write_function(FILE *fp, char *name, int n_vars);
void write_return(FILE *fp);

#endif
