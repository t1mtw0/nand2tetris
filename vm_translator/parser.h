#ifndef PARSER_H
#define PARSER_H

#include "common.h"

enum CommandType {
    C_NULL,
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
};

struct Parser {
    char *fi;
    FILE *fp;
    bool has_more_lines;
    enum CommandType command_type;
    char *arg1;
    char *arg2;
};

struct Parser new_parser(char *proj_dir, char *fi);
bool advance(struct Parser *p);
enum CommandType command_type(struct Parser *p);
char *arg1(struct Parser *p);
char *arg2(struct Parser *p);
void end_parser(struct Parser *p);

#endif
