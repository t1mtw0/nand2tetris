#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "common.h"

enum TokenType {
    T_NONE,
    T_KEYWORD,
    T_SYMBOL,
    T_IDENTIFIER,
    T_INT_CONST,
    T_STRING_CONST,
};

enum Keywords {
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
    FILE *fp;
};

struct Tokenizer new_tokenizer(FILE *fp);
bool has_more_tokens(Tokenizer *t);
void advance(Tokenizer *t);
enum TokenType token_type(Tokenizer *t);
enum Keyword keyword(Tokenizer *t);
char sym(Tokenizer *t);
char *identifier(Tokenizer *t);
int int_val(Tokenizer *t);
char *string_val(Tokenizer *t);

#endif