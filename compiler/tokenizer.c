#include "tokenizer.h"

#include "common.h"
#include <ctype.h>
#include <string.h>

static bool has_more_tokens(struct Tokenizer *t) {
    if (*t->current == '\0')
        return false;
    return true;
}

static char advance(struct Tokenizer *t) {
    t->current++;
    return t->current[-1];
}

static char peek(struct Tokenizer *t) { return *t->current; }

static void skip_whitespace(struct Tokenizer *t) {
    while (isspace(peek(t))) {
        advance(t);
    }
}

static enum Keywords check_keyword(struct Tokenizer *t, char *kw, int l,
                                   enum Keywords rkw) {
    if ((int)(t->current - t->start) == l && !strncmp(t->start, kw, l))
        return rkw;
    return K_NONE;
}

static enum Keywords is_keyword(struct Tokenizer *t) {
    switch (*t->start) {
    case 'c':
        switch (t->start[1]) {
        case 'l':
            return check_keyword(t, "class", 5, K_CLASS);
        case 'o':
            return check_keyword(t, "constructor", 11, K_CONSTRUCTOR);
        case 'h':
            return check_keyword(t, "char", 4, K_CHAR);
        }
    case 'f':
        switch (t->start[1]) {
        case 'i':
            return check_keyword(t, "field", 5, K_FIELD);
        case 'u':
            return check_keyword(t, "function", 8, K_FUNCTION);
        case 'a':
            return check_keyword(t, "false", 5, K_FALSE);
        }
    case 'm':
        return check_keyword(t, "method", 6, K_METHOD);
    case 's':
        return check_keyword(t, "static", 6, K_STATIC);
    case 'v':
        switch (t->start[1]) {
        case 'a':
            return check_keyword(t, "var", 3, K_VAR);
        case 'o':
            return check_keyword(t, "void", 4, K_VOID);
        }
    case 'i':
        switch (t->start[1]) {
        case 'f':
            return check_keyword(t, "if", 2, K_IF);
        case 'n':
            return check_keyword(t, "int", 3, K_INT);
        }
    case 'b':
        return check_keyword(t, "boolean", 7, K_BOOLEAN);
    case 't':
        switch (t->start[1]) {
        case 'h':
            return check_keyword(t, "this", 4, K_THIS);
        case 'r':
            return check_keyword(t, "true", 4, K_TRUE);
        }
    case 'n':
        return check_keyword(t, "null", 4, K_NULL);
    case 'l':
        return check_keyword(t, "let", 3, K_LET);
    case 'd':
        return check_keyword(t, "do", 2, K_DO);
    case 'e':
        return check_keyword(t, "else", 4, K_ELSE);
    case 'w':
        return check_keyword(t, "while", 5, K_WHILE);
    case 'r':
        return check_keyword(t, "return", 6, K_RETURN);
    }
    return K_NONE;
}

static struct Token make_token(struct Tokenizer *t, enum TokenType type,
                               enum Keywords kw, enum Symbols sym) {
    int length = (int)(t->current - t->start);
    struct Token tok = {.begin = t->start,
                        .length = length,
                        .type = type,
                        .kw = kw,
                        .sym = sym};
    return tok;
}

static struct Token ident_or_keyword(struct Tokenizer *t) {
    while (isalpha(peek(t)) || isdigit(peek(t)) || peek(t) == '_')
        advance(t);
    enum Keywords kt = is_keyword(t);
    if (kt == K_NONE)
        return make_token(t, T_IDENTIFIER, kt, S_NONE);
    else
        return make_token(t, T_KEYWORD, kt, S_NONE);
}

static struct Token int_const(struct Tokenizer *t) {
    while (isdigit(peek(t)))
        advance(t);
    return make_token(t, T_INT_CONST, K_NONE, S_NONE);
}

static struct Token string_const(struct Tokenizer *t) {
    t->start++;
    while (peek(t) != '\"')
        advance(t);
    struct Token tok = make_token(t, T_STRING_CONST, K_NONE, S_NONE);
    advance(t);
    return tok;
}

struct Tokenizer new_tokenizer(FILE *fp) {
    fseek(fp, 0L, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    char *buffer = malloc(fsize + 1);
    if (!buffer) {
        fclose(fp);
        fprintf(stderr, "%s", "Memory allocation failed.");
        exit(1);
    }

    int read = fread(buffer, fsize, 1, fp);
    if (read != 1) {
        fclose(fp);
        fprintf(stderr, "%s", "Reading file failed.");
        exit(1);
    }

    buffer[fsize] = '\0';

    fclose(fp);

    struct Tokenizer t = {.source = buffer, .start = buffer, .current = buffer};
    return t;
}

struct Token scan_token(struct Tokenizer *t) {
    skip_whitespace(t);
    t->start = t->current;

    if (!has_more_tokens(t))
        return make_token(t, T_EOF, K_NONE, S_NONE);

    char c = advance(t);

    if (isalpha(c) || c == '_')
        return ident_or_keyword(t);
    else if (isdigit(c))
        return int_const(t);
    else if (c == '\"')
        return string_const(t);

    switch (c) {
    case '{':
        return make_token(t, T_SYMBOL, K_NONE, S_LEFT_BRACE);
    case '}':
        return make_token(t, T_SYMBOL, K_NONE, S_RIGHT_BRACE);
    case '(':
        return make_token(t, T_SYMBOL, K_NONE, S_LEFT_PAREN);
    case ')':
        return make_token(t, T_SYMBOL, K_NONE, S_RIGHT_PAREN);
    case '[':
        return make_token(t, T_SYMBOL, K_NONE, S_LEFT_BRACKET);
    case ']':
        return make_token(t, T_SYMBOL, K_NONE, S_RIGHT_BRACKET);
    case '.':
        return make_token(t, T_SYMBOL, K_NONE, S_DOT);
    case ',':
        return make_token(t, T_SYMBOL, K_NONE, S_COMMA);
    case ';':
        return make_token(t, T_SYMBOL, K_NONE, S_SEMICOLON);
    case '+':
        return make_token(t, T_SYMBOL, K_NONE, S_PLUS);
    case '-':
        return make_token(t, T_SYMBOL, K_NONE, S_SUB);
    case '*':
        return make_token(t, T_SYMBOL, K_NONE, S_MULT);
    case '/':
        if (peek(t) == '/') {
            while (peek(t) != '\n' && has_more_tokens(t))
                advance(t);
            advance(t);
            return scan_token(t);
        } else if (peek(t) == '*') {
            advance(t);
            while (has_more_tokens(t) &&
                   (peek(t) != '*' || t->current[1] != '/'))
                advance(t);
            if (!has_more_tokens(t)) {
                fprintf(stderr, "%s", "Multiline comment is not closed.");
                exit(1);
            }
            advance(t);
            advance(t);
            return scan_token(t);
        }
        return make_token(t, T_SYMBOL, K_NONE, S_DIV);
    case '&':
        return make_token(t, T_SYMBOL, K_NONE, S_AND);
    case '|':
        return make_token(t, T_SYMBOL, K_NONE, S_OR);
    case '<':
        return make_token(t, T_SYMBOL, K_NONE, S_LESS_THAN);
    case '>':
        return make_token(t, T_SYMBOL, K_NONE, S_GREATER_THAN);
    case '=':
        return make_token(t, T_SYMBOL, K_NONE, S_EQUAL);
    case '~':
        return make_token(t, T_SYMBOL, K_NONE, S_TILDE);
    default:
        fprintf(stderr, "%s", "Unrecognized token while scanning.");
        exit(1);
    }
}

void end_tokenizer(struct Tokenizer *t) { free(t->source); }
