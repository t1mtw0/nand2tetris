#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include "common.h"
#include "parser.h"

struct CodeWriter {
    char *fn;
    FILE *fp;
    char *file_name;
    char *cur_fn;
    int jmp_label;
    int fn_ret_label;
};

struct CodeWriter new_code_writer(char *proj_dir, char *prog_name);
void set_file_name(struct CodeWriter *cw, char *file_name);
void write_arithmetic(struct CodeWriter *cw, char *command);
void write_push_pop(struct CodeWriter *cw, enum CommandType command,
                    char *segment, int index);
void write_label(struct CodeWriter *cw, char *label);
void write_goto(struct CodeWriter *cw, char *label);
void write_if(struct CodeWriter *cw, char *label);
void write_function(struct CodeWriter *cw, char *function_name, int n_vars);
void write_call(struct CodeWriter *cw, char *function_name, int n_args);
void write_return(struct CodeWriter *cw);
void end_code_writer(struct CodeWriter *cw);

#endif
