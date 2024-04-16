#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "common.h"
#include "table.h"
#include "tokenizer.h"

struct CompilationEngine {
    char *class_name;
    struct Tokenizer *t;
    FILE *fop;
    struct Token current;
    struct Token previous;
    struct Table sym_t_c;
    struct Table sym_t_sr;
    int lbl_count;
};

struct CompilationEngine new_compilation_engine(struct Tokenizer *t, FILE *fop);
void compile(struct CompilationEngine *ce);
void compile_class(struct CompilationEngine *ce);
void compile_class_var_dec(struct CompilationEngine *ce);
void compile_subroutine(struct CompilationEngine *ce);
void compile_par_list(struct CompilationEngine *ce);
void compile_subroutine_body(struct CompilationEngine *ce, char *name,
                             enum Keywords sub_type);
void compile_var_dec(struct CompilationEngine *ce);
void compile_stmts(struct CompilationEngine *ce);
void compile_let(struct CompilationEngine *ce);
void compile_if(struct CompilationEngine *ce);
void compile_while(struct CompilationEngine *ce);
void compile_do(struct CompilationEngine *ce);
void compile_return(struct CompilationEngine *ce);
void compile_expr(struct CompilationEngine *ce);
void compile_term(struct CompilationEngine *ce);
int compile_expr_list(struct CompilationEngine *ce);

#endif
