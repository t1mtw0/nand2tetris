#include "code_writer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

struct CodeWriter new_code_writer(FILE *fp) {
    struct CodeWriter cw = {.fp = fp};
    return cw;
}

void write_arithmetic(struct CodeWriter *cw, char *command) {}

void write_push_pop(struct CodeWriter *cw, enum CommandType command,
                    char *segment, int index) {}

void end_code_writer(struct CodeWriter *cw) { fclose(cw->fp); }
