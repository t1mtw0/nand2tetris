#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include "common.h"
#include "parser.h"

struct CodeWriter {
    char *prog_name;
    FILE *fp;
    int jmp_label;
};

struct CodeWriter new_code_writer(char *prog_name, FILE *fp);
void write_arithmetic(struct CodeWriter *cw, char *command);
void write_push_pop(struct CodeWriter *cw, enum CommandType command,
                    char *segment, int index);
void end_code_writer(struct CodeWriter *cw);

#endif
