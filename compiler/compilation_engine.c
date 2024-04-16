#include "compilation_engine.h"
#include "table.h"
#include "tokenizer.h"
#include "vm_writer.h"
#include <string.h>

static void advance(struct CompilationEngine *ce) {
    ce->previous = ce->current;
    ce->current = scan_token(ce->t);
}

static void process(struct CompilationEngine *ce, enum TokenType type,
                    enum Keywords kw, enum Symbols sym, char *err_msg) {
    if (ce->current.type != type || ce->current.kw != kw ||
        ce->current.sym != sym) {
        fprintf(stderr, "%s", err_msg);
        exit(1);
    }
    advance(ce);
}

static bool match(struct CompilationEngine *ce, enum TokenType type,
                  enum Keywords kw, enum Symbols sym) {
    if (ce->current.type == type && ce->current.kw == kw &&
        ce->current.sym == sym) {
        advance(ce);
        return true;
    }
    return false;
}

static bool check(struct CompilationEngine *ce, enum TokenType type,
                  enum Keywords kw, enum Symbols sym) {
    if (ce->current.type != type || ce->current.kw != kw ||
        ce->current.sym != sym)
        return false;
    return true;
}

static char *extract_identifier(struct CompilationEngine *ce) {
    char *id = malloc(sizeof(char) * (ce->previous.length + 1));
    strncpy(id, ce->previous.begin, ce->previous.length);
    id[ce->previous.length] = '\0';
    return id;
}

static char *make_sr_name(struct CompilationEngine *ce, char *class_name,
                          char *name) {
    char *class_n;
    if (class_name)
        class_n = class_name;
    else
        class_n = ce->class_name;
    int class_n_l = strlen(class_n);
    int name_l = strlen(name);
    char *sr_name = malloc(sizeof(char) * (name_l + class_n_l + 2));
    strncpy(sr_name, class_n, class_n_l);
    strncpy(sr_name + class_n_l, ".", 1);
    strncpy(sr_name + class_n_l + 1, name, name_l);
    sr_name[class_n_l + name_l + 1] = '\0';
    return sr_name;
}

static char *type_to_str(struct CompilationEngine *ce, enum Keywords t) {
    char *type = NULL;
    switch (t) {
    case K_BOOLEAN:
        type = malloc(sizeof(char) * 8);
        strncpy(type, "boolean", 8);
        break;
    case K_CHAR:
        type = malloc(sizeof(char) * 5);
        strncpy(type, "char", 5);
        break;
    case K_INT:
        type = malloc(sizeof(char) * 4);
        strncpy(type, "int", 4);
        break;
    case K_NONE:
        type = extract_identifier(ce);
        break;
    default:
        break;
    }
    return type;
}

static enum Keywords process_type(struct CompilationEngine *ce) {
    if (match(ce, T_KEYWORD, K_INT, S_NONE))
        return K_INT;
    else if (match(ce, T_KEYWORD, K_BOOLEAN, S_NONE))
        return K_BOOLEAN;
    else if (match(ce, T_KEYWORD, K_CHAR, S_NONE))
        return K_CHAR;
    else if (match(ce, T_IDENTIFIER, K_NONE, S_NONE))
        return K_NONE;
    else
        fprintf(stderr, "%s", "Expected type."), exit(1);
}

static enum Keywords process_sub_type(struct CompilationEngine *ce) {
    if (match(ce, T_KEYWORD, K_CONSTRUCTOR, S_NONE))
        return K_CONSTRUCTOR;
    else if (match(ce, T_KEYWORD, K_METHOD, S_NONE))
        return K_METHOD;
    else if (match(ce, T_KEYWORD, K_FUNCTION, S_NONE))
        return K_FUNCTION;
    else
        fprintf(stderr, "%s",
                "Expected 'constructor', 'method', or 'function'."),
            exit(1);
}

struct CompilationEngine new_compilation_engine(struct Tokenizer *t,
                                                FILE *fop) {
    struct Table sym_t_c = new_table();
    struct Table sym_t_sr = new_table();
    struct Token tok = {.begin = NULL,
                        .kw = K_NONE,
                        .length = 0,
                        .sym = S_NONE,
                        .type = T_NONE};
    struct CompilationEngine ce = {.class_name = NULL,
                                   .t = t,
                                   .fop = fop,
                                   .current = tok,
                                   .previous = tok,
                                   .sym_t_c = sym_t_c,
                                   .sym_t_sr = sym_t_sr};
    return ce;
}

void compile(struct CompilationEngine *ce) {
    advance(ce);
    compile_class(ce);
}

void compile_class(struct CompilationEngine *ce) {
    process(ce, T_KEYWORD, K_CLASS, S_NONE, "Expected 'class' keyword.");
    process(ce, T_IDENTIFIER, K_NONE, S_NONE,
            "Expected identifier after 'class'.");
    char *class_name = extract_identifier(ce);
    ce->class_name = class_name;
    process(ce, T_SYMBOL, K_NONE, S_LEFT_BRACE,
            "Expected '{' after class name.");
    while (check(ce, T_KEYWORD, K_FIELD, S_NONE) ||
           check(ce, T_KEYWORD, K_STATIC, S_NONE))
        compile_class_var_dec(ce);
#ifdef DEBUG
    printf("%s\n\n", ce->class_name);
    print_table(&ce->sym_t_c);
#endif
    while (!check(ce, T_SYMBOL, K_NONE, S_RIGHT_BRACE))
        compile_subroutine(ce);
    process(ce, T_SYMBOL, K_NONE, S_RIGHT_BRACE,
            "Expected '}' after class body.");
    if (ce->current.type != T_EOF) {
        fprintf(stderr, "%s", "Only one class is permitted per file.");
        exit(1);
    }
}

void compile_class_var_dec(struct CompilationEngine *ce) {
    enum Kinds k = KD_NONE;
    if (match(ce, T_KEYWORD, K_FIELD, S_NONE))
        k = KD_FIELD;
    else if (match(ce, T_KEYWORD, K_STATIC, S_NONE))
        k = KD_STATIC;
    enum Keywords t = process_type(ce);
    char *type = type_to_str(ce, t);
    process(ce, T_IDENTIFIER, K_NONE, S_NONE,
            "Expected identifier after type.");
    char *name = extract_identifier(ce);
    define(&ce->sym_t_c, name, type, k);
    while (match(ce, T_SYMBOL, K_NONE, S_COMMA)) {
        process(ce, T_IDENTIFIER, K_NONE, S_NONE, "Expected identifier.");
        name = extract_identifier(ce);
        int type_l = strlen(type);
        char *type_n = malloc(sizeof(char) * (type_l + 1));
        strncpy(type_n, type, type_l);
        type_n[type_l] = '\0';
        define(&ce->sym_t_c, name, type_n, k);
    }
    process(ce, T_SYMBOL, K_NONE, S_SEMICOLON,
            "Expected ';' after variable declaration.");
}

void compile_subroutine(struct CompilationEngine *ce) {
    reset(&ce->sym_t_sr);
    enum Keywords sub_type = process_sub_type(ce);
    if (!match(ce, T_KEYWORD, K_VOID, S_NONE))
        process_type(ce);
    process(ce, T_IDENTIFIER, K_NONE, S_NONE,
            "Expected identifier in subroutine declaration.");
    char *name = extract_identifier(ce);
    if (sub_type == K_METHOD) {
        char *this_str = malloc(sizeof(char) * 5);
        strncpy(this_str, "this", 5);
        int class_name_l = strlen(ce->class_name);
        char *class_str = malloc(sizeof(char) * (class_name_l + 1));
        strncpy(class_str, ce->class_name, class_name_l);
        class_str[class_name_l] = '\0';
        define(&ce->sym_t_sr, this_str, class_str, KD_ARG);
    }
    process(ce, T_SYMBOL, K_NONE, S_LEFT_PAREN,
            "Expected '(' after subroutine declaration.");
    if (!check(ce, T_SYMBOL, K_NONE, S_RIGHT_PAREN))
        compile_par_list(ce);
    process(ce, T_SYMBOL, K_NONE, S_RIGHT_PAREN,
            "Expected ')' after subroutine declaration.");
    compile_subroutine_body(ce, name, sub_type);
}

void compile_par_list(struct CompilationEngine *ce) {
    enum Keywords t = process_type(ce);
    process(ce, T_IDENTIFIER, K_NONE, S_NONE,
            "Expected identifier after type in parameter list.");
    char *name = extract_identifier(ce);
    char *type = type_to_str(ce, t);
    define(&ce->sym_t_sr, name, type, KD_ARG);
    while (match(ce, T_SYMBOL, K_NONE, S_COMMA)) {
        t = process_type(ce);
        process(ce, T_IDENTIFIER, K_NONE, S_NONE,
                "Expected identifier after ','.");
        name = extract_identifier(ce);
        type = type_to_str(ce, t);
        define(&ce->sym_t_sr, name, type, KD_ARG);
    }
}

void compile_subroutine_body(struct CompilationEngine *ce, char *name,
                             enum Keywords sub_type) {
    process(ce, T_SYMBOL, K_NONE, S_LEFT_BRACE,
            "Expected '{' before subroutine body.");
    while (match(ce, T_KEYWORD, K_VAR, S_NONE))
        compile_var_dec(ce);
    char *sr_name = make_sr_name(ce, NULL, name);
#ifdef DEBUG
    printf("Subroutine: %s\n\n", sr_name);
    print_table(&ce->sym_t_sr);
#endif
    switch (sub_type) {
    case K_CONSTRUCTOR:
        write_function(ce->fop, sr_name, ce->sym_t_sr.n_var);
        write_push(ce->fop, SEG_CONSTANT, ce->sym_t_c.n_field);
        write_call(ce->fop, "Memory.alloc", 1);
        write_pop(ce->fop, SEG_POINTER, 0);
        break;
    case K_METHOD:
        write_function(ce->fop, sr_name, ce->sym_t_sr.n_var);
        write_push(ce->fop, SEG_ARGUMENT, 0);
        write_pop(ce->fop, SEG_POINTER, 0);
        break;
    case K_FUNCTION:
        write_function(ce->fop, sr_name, ce->sym_t_sr.n_var);
        break;
    default:
        break;
    }
    free(sr_name);
    compile_stmts(ce);
    process(ce, T_SYMBOL, K_NONE, S_RIGHT_BRACE,
            "Expected '}' after subroutine body.");
}

void compile_var_dec(struct CompilationEngine *ce) {
    enum Keywords t = process_type(ce);
    char *type = type_to_str(ce, t);
    process(ce, T_IDENTIFIER, K_NONE, S_NONE,
            "Expected identifier after 'var'.");
    char *name = extract_identifier(ce);
    define(&ce->sym_t_sr, name, type, KD_VAR);
    while (match(ce, T_SYMBOL, K_NONE, S_COMMA)) {
        process(ce, T_IDENTIFIER, K_NONE, S_NONE,
                "Expected identifier after ','.");
        name = extract_identifier(ce);
        int type_l = strlen(type);
        char *type_n = malloc(sizeof(char) * (type_l + 1));
        strncpy(type_n, type, type_l);
        type_n[type_l] = '\0';
        define(&ce->sym_t_sr, name, type_n, KD_VAR);
    }
    process(ce, T_SYMBOL, K_NONE, S_SEMICOLON,
            "Expected ';' after variable declaration.");
}

void compile_stmts(struct CompilationEngine *ce) {
    while (true) {
        if (match(ce, T_KEYWORD, K_LET, S_NONE)) {
            compile_let(ce);
        } else if (match(ce, T_KEYWORD, K_IF, S_NONE)) {
            compile_if(ce);
        } else if (match(ce, T_KEYWORD, K_WHILE, S_NONE)) {
            compile_while(ce);
        } else if (match(ce, T_KEYWORD, K_DO, S_NONE)) {
            compile_do(ce);
        } else if (match(ce, T_KEYWORD, K_RETURN, S_NONE)) {
            compile_return(ce);
        } else {
            break;
        }
    }
}

void compile_let(struct CompilationEngine *ce) {
    process(ce, T_IDENTIFIER, K_NONE, S_NONE,
            "Expected identifier after 'let'.");
    char *name = extract_identifier(ce);
    enum Kinds k = kind_of(&ce->sym_t_sr, name);
    if (k == KD_NONE)
        k = kind_of(&ce->sym_t_c, name);
    int index = index_of(&ce->sym_t_sr, name);
    if (index == -1)
        index = index_of(&ce->sym_t_c, name);
    enum Segments seg;
    switch (k) {
    case KD_ARG:
        seg = SEG_ARGUMENT;
        break;
    case KD_FIELD:
        seg = SEG_THIS;
        break;
    case KD_STATIC:
        seg = SEG_STATIC;
        break;
    case KD_VAR:
        seg = SEG_LOCAL;
        break;
    default:
        break;
    }
    if (match(ce, T_SYMBOL, K_NONE, S_LEFT_BRACKET)) {
        write_push(ce->fop, seg, index);
        compile_expr(ce);
        write_arithmetic(ce->fop, A_ADD);
        process(ce, T_SYMBOL, K_NONE, S_RIGHT_BRACKET,
                "Expected ']' after expression.");
        process(ce, T_SYMBOL, K_NONE, S_EQUAL,
                "Expected '=' in let statement.");
        compile_expr(ce);
        write_pop(ce->fop, SEG_TEMP, 0);
        write_pop(ce->fop, SEG_POINTER, 1);
        write_push(ce->fop, SEG_TEMP, 0);
        write_pop(ce->fop, SEG_THAT, 0);
    } else {
        process(ce, T_SYMBOL, K_NONE, S_EQUAL,
                "Expected '=' in let statement.");
        compile_expr(ce);
        write_pop(ce->fop, seg, index);
    }
    process(ce, T_SYMBOL, K_NONE, S_SEMICOLON,
            "Expected ';' after variable declaration.");
}

void compile_if(struct CompilationEngine *ce) {
    process(ce, T_SYMBOL, K_NONE, S_LEFT_PAREN, "Expected '(' after 'if'.");
    compile_expr(ce);
    write_arithmetic(ce->fop, A_NOT);
    char lbl1[50];
    snprintf(lbl1, 50, "L%d", ce->lbl_count++);
    write_if(ce->fop, lbl1);
    process(ce, T_SYMBOL, K_NONE, S_RIGHT_PAREN,
            "Expected ')' after expression.");
    process(ce, T_SYMBOL, K_NONE, S_LEFT_BRACE,
            "Expected '{' after if condition.");
    compile_stmts(ce);
    char lbl2[50];
    snprintf(lbl2, 50, "L%d", ce->lbl_count++);
    write_goto(ce->fop, lbl2);
    write_label(ce->fop, lbl1);
    process(ce, T_SYMBOL, K_NONE, S_RIGHT_BRACE, "Expected '}' after if body.");
    if (match(ce, T_KEYWORD, K_ELSE, S_NONE)) {
        process(ce, T_SYMBOL, K_NONE, S_LEFT_BRACE,
                "Expected '{' after 'else'.");
        compile_stmts(ce);
        process(ce, T_SYMBOL, K_NONE, S_RIGHT_BRACE,
                "Expected '}' after else body.");
    }
    write_label(ce->fop, lbl2);
}

void compile_while(struct CompilationEngine *ce) {
    process(ce, T_SYMBOL, K_NONE, S_LEFT_PAREN, "Expected '(' after 'if'.");
    char lbl1[50];
    snprintf(lbl1, 50, "L%d", ce->lbl_count++);
    write_label(ce->fop, lbl1);
    compile_expr(ce);
    write_arithmetic(ce->fop, A_NOT);
    char lbl2[50];
    snprintf(lbl2, 50, "L%d", ce->lbl_count++);
    write_if(ce->fop, lbl2);
    process(ce, T_SYMBOL, K_NONE, S_RIGHT_PAREN,
            "Expected ')' after expression.");
    process(ce, T_SYMBOL, K_NONE, S_LEFT_BRACE,
            "Expected '{' in if statement.");
    compile_stmts(ce);
    write_goto(ce->fop, lbl1);
    process(ce, T_SYMBOL, K_NONE, S_RIGHT_BRACE, "Expected '}' after if body.");
    write_label(ce->fop, lbl2);
}

void compile_do(struct CompilationEngine *ce) {
    compile_expr(ce);
    write_pop(ce->fop, SEG_TEMP, 0);
    process(ce, T_SYMBOL, K_NONE, S_SEMICOLON,
            "Expected ';' after return statement.");
}

void compile_return(struct CompilationEngine *ce) {
    if (!check(ce, T_SYMBOL, K_NONE, S_SEMICOLON))
        compile_expr(ce);
    else
        write_push(ce->fop, SEG_CONSTANT, 0);
    write_return(ce->fop);
    process(ce, T_SYMBOL, K_NONE, S_SEMICOLON,
            "Expected ';' after return statement.");
}

void compile_expr(struct CompilationEngine *ce) {
    compile_term(ce);
    while (true) {
        if (match(ce, T_SYMBOL, K_NONE, S_PLUS)) {
            compile_term(ce);
            write_arithmetic(ce->fop, A_ADD);
        } else if (match(ce, T_SYMBOL, K_NONE, S_SUB)) {
            compile_term(ce);
            write_arithmetic(ce->fop, A_SUB);
        } else if (match(ce, T_SYMBOL, K_NONE, S_MULT)) {
            compile_term(ce);
            write_call(ce->fop, "Math.multiply", 2);
        } else if (match(ce, T_SYMBOL, K_NONE, S_DIV)) {
            compile_term(ce);
            write_call(ce->fop, "Math.divide", 2);
        } else if (match(ce, T_SYMBOL, K_NONE, S_AND)) {
            compile_term(ce);
            write_arithmetic(ce->fop, A_AND);
        } else if (match(ce, T_SYMBOL, K_NONE, S_OR)) {
            compile_term(ce);
            write_arithmetic(ce->fop, A_OR);
        } else if (match(ce, T_SYMBOL, K_NONE, S_LESS_THAN)) {
            compile_term(ce);
            write_arithmetic(ce->fop, A_LT);
        } else if (match(ce, T_SYMBOL, K_NONE, S_GREATER_THAN)) {
            compile_term(ce);
            write_arithmetic(ce->fop, A_GT);
        } else if (match(ce, T_SYMBOL, K_NONE, S_EQUAL)) {
            compile_term(ce);
            write_arithmetic(ce->fop, A_EQ);
        } else {
            break;
        }
    }
}

void compile_term(struct CompilationEngine *ce) {
    if (match(ce, T_INT_CONST, K_NONE, S_NONE)) {
        write_push(ce->fop, SEG_CONSTANT,
                   (int)strtol(extract_identifier(ce), NULL, 10));
    } else if (match(ce, T_STRING_CONST, K_NONE, S_NONE)) {
        char *name = extract_identifier(ce);
        write_push(ce->fop, SEG_CONSTANT, strlen(name));
        write_call(ce->fop, "String.new", 1);
        for (int i = 0; i < strlen(name); i++) {
            write_push(ce->fop, SEG_CONSTANT, (int)(name[i]));
            write_call(ce->fop, "String.appendChar", 2);
        }
    } else if (match(ce, T_KEYWORD, K_TRUE, S_NONE)) {
        write_push(ce->fop, SEG_CONSTANT, 1);
    } else if (match(ce, T_KEYWORD, K_FALSE, S_NONE)) {
        write_push(ce->fop, SEG_CONSTANT, 0);
    } else if (match(ce, T_KEYWORD, K_NULL, S_NONE)) {
        write_push(ce->fop, SEG_CONSTANT, 0);
    } else if (match(ce, T_KEYWORD, K_THIS, S_NONE)) {
        write_push(ce->fop, SEG_POINTER, 0);
    } else if (match(ce, T_IDENTIFIER, K_NONE, S_NONE)) {
        char *name = extract_identifier(ce);
        enum Kinds k = kind_of(&ce->sym_t_sr, name);
        if (k == KD_NONE)
            k = kind_of(&ce->sym_t_c, name);
        int index = index_of(&ce->sym_t_sr, name);
        if (index == -1)
            index = index_of(&ce->sym_t_c, name);
        enum Segments seg;
        switch (k) {
        case KD_ARG:
            seg = SEG_ARGUMENT;
            break;
        case KD_FIELD:
            seg = SEG_THIS;
            break;
        case KD_STATIC:
            seg = SEG_STATIC;
            break;
        case KD_VAR:
            seg = SEG_LOCAL;
            break;
        case KD_NONE:
            break;
        }
        if (match(ce, T_SYMBOL, K_NONE, S_LEFT_PAREN)) {
            write_push(ce->fop, SEG_POINTER, 0);
            int n_args = compile_expr_list(ce);
            process(ce, T_SYMBOL, K_NONE, S_RIGHT_PAREN,
                    "Expected ')' after expression list.");
            char *call_name = make_sr_name(ce, NULL, name);
            write_call(ce->fop, call_name, n_args + 1);
        } else if (match(ce, T_SYMBOL, K_NONE, S_DOT)) {
            process(ce, T_IDENTIFIER, K_NONE, S_NONE,
                    "Expected subroutine name after '.'.");
            char *sr_name = extract_identifier(ce);
            process(ce, T_SYMBOL, K_NONE, S_LEFT_PAREN,
                    "Expected '(' after subroutine name.");
            if (kind_of(&ce->sym_t_c, name) != KD_NONE ||
                kind_of(&ce->sym_t_sr, name) != KD_NONE)
                write_push(ce->fop, seg, index);
            int n_args = compile_expr_list(ce);
            process(ce, T_SYMBOL, K_NONE, S_RIGHT_PAREN,
                    "Expected ')' after expression list.");
            if (kind_of(&ce->sym_t_c, name) == KD_NONE &&
                kind_of(&ce->sym_t_sr, name) == KD_NONE) {
                char *call_name = make_sr_name(ce, name, sr_name);
                write_call(ce->fop, call_name, n_args);
            } else if (kind_of(&ce->sym_t_c, name) != KD_NONE) {
                char *class_name = type_of(&ce->sym_t_c, name);
                char *call_name = make_sr_name(ce, class_name, sr_name);
                write_call(ce->fop, call_name, n_args + 1);
            } else if (kind_of(&ce->sym_t_sr, name) != KD_NONE) {
                char *class_name = type_of(&ce->sym_t_sr, name);
                char *call_name = make_sr_name(ce, class_name, sr_name);
                write_call(ce->fop, call_name, n_args + 1);
            }
        } else if (match(ce, T_SYMBOL, K_NONE, S_LEFT_BRACKET)) {
            write_push(ce->fop, seg, index);
            compile_expr(ce);
            write_arithmetic(ce->fop, A_ADD);
            write_pop(ce->fop, SEG_POINTER, 1);
            write_push(ce->fop, SEG_THAT, 0);
            process(ce, T_SYMBOL, K_NONE, S_RIGHT_BRACKET,
                    "Expected ']' after expression.");
        } else {
            write_push(ce->fop, seg, index);
        }
    } else if (match(ce, T_SYMBOL, K_NONE, S_LEFT_PAREN)) {
        compile_expr(ce);
        process(ce, T_SYMBOL, K_NONE, S_RIGHT_PAREN,
                "Expected ')' after expression.");
    } else if (match(ce, T_SYMBOL, K_NONE, S_SUB)) {
        compile_term(ce);
        write_arithmetic(ce->fop, A_NEG);
    } else if (match(ce, T_SYMBOL, K_NONE, S_TILDE)) {
        compile_term(ce);
        write_arithmetic(ce->fop, A_NOT);
    } else {
        fprintf(stderr, "%s", "Expected term.");
        exit(1);
    }
}

int compile_expr_list(struct CompilationEngine *ce) {
    if (!check(ce, T_INT_CONST, K_NONE, S_NONE) &&
        !check(ce, T_STRING_CONST, K_NONE, S_NONE) &&
        !check(ce, T_IDENTIFIER, K_NONE, S_NONE) &&
        !check(ce, T_KEYWORD, K_TRUE, S_NONE) &&
        !check(ce, T_KEYWORD, K_FALSE, S_NONE) &&
        !check(ce, T_KEYWORD, K_NULL, S_NONE) &&
        !check(ce, T_KEYWORD, K_THIS, S_NONE) &&
        !check(ce, T_SYMBOL, K_NONE, S_LEFT_PAREN) &&
        !check(ce, T_SYMBOL, K_NONE, S_SUB) &&
        !check(ce, T_SYMBOL, K_NONE, S_TILDE))
        return 0;
    compile_expr(ce);
    int count = 1;
    while (match(ce, T_SYMBOL, K_NONE, S_COMMA)) {
        compile_expr(ce);
        count++;
    }
    return count;
}
