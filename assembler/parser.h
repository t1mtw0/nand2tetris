#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include <stdio.h>
#include <stdlib.h>

enum InstrType {
    A_INSTRUCTION,
    C_INSTRUCTION,
    L_INSTRUCTION,
};

struct Parser {
    FILE *fp;
    bool hasMoreLines;
    enum InstrType type;
    char *sym;
    char *dest;
    char *comp;
    char *jump;
};

struct Parser newParser(FILE *fp);
bool advance(struct Parser *parser);
enum InstrType instructionType(struct Parser *parser);
char *symbol(struct Parser *parser);
char *dest(struct Parser *parser);
char *comp(struct Parser *parser);
char *jump(struct Parser *parser);
void freeParser(struct Parser *parser);

#endif
