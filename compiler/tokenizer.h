#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "common.h"

enum TokenType {
    T_NONE,
    T_EOF,
    T_KEYWORD,
    T_SYMBOL,
    T_IDENTIFIER,
    T_INT_CONST,
    T_STRING_CONST,
};

enum Keywords {
    K_NONE,
    K_CLASS,
    K_METHOD,
    K_FUNCTION,
    K_CONSTRUCTOR,
    K_INT,
    K_BOOLEAN,
    K_CHAR,
    K_VOID,
    K_VAR,
    K_STATIC,
    K_FIELD,
    K_LET,
    K_DO,
    K_IF,
    K_ELSE,
    K_WHILE,
    K_RETURN,
    K_TRUE,
    K_FALSE,
    K_NULL,
    K_THIS,
};

struct Tokenizer {
    char* source;
    char* start;
    char* current;
};

struct Token {
    enum TokenType type;
    enum Keywords kw;
    const char* begin;
    int length;
};

struct Tokenizer new_tokenizer(FILE *fp);
struct Token scan_token(struct Tokenizer *t);
void end_tokenizer(struct Tokenizer *t);

#endif
