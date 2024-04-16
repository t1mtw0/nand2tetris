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

enum Symbols {
    S_NONE,
    S_LEFT_BRACE,
    S_RIGHT_BRACE,
    S_LEFT_PAREN,
    S_RIGHT_PAREN,
    S_LEFT_BRACKET,
    S_RIGHT_BRACKET,
    S_LESS_THAN,
    S_GREATER_THAN,
    S_EQUAL,
    S_DOT,
    S_COMMA,
    S_SEMICOLON,
    S_PLUS,
    S_DIV,
    S_SUB,
    S_MULT,
    S_AND,
    S_OR,
    S_TILDE,
};

struct Tokenizer {
    char *source;
    char *start;
    char *current;
};

struct Token {
    const char *begin;
    int length;
    enum TokenType type;
    enum Keywords kw;
    enum Symbols sym;
};

struct Tokenizer new_tokenizer(FILE *fp);
struct Token scan_token(struct Tokenizer *t);
void end_tokenizer(struct Tokenizer *t);

#endif
