#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "common.h"

struct CompilationEngine {
    FILE *fip;
    FILE *fop;
};

struct CompilationEngine new_compilation_engine(FILE *fip, FILE *fop);
void compile_class(CompilationEngine *ce);
void compile_class_var_dec(CompilationEngine *ce);
void compile_subroutine(CompilationEngine *ce);
void compile_par_list(CompilationEngine *ce);
void compile_subroutine_body(CompilationEngine *ce);
void compile_var_dec(CompilationEngine *ce);
void compile_stmts(CompilationEngine *ce);
void compile_let(CompilationEngine *ce);
void compile_if(CompilationEngine *ce);
void compile_while(CompilationEngine *ce);
void compile_do(CompilationEngine *ce);
void compile_return(CompilationEngine *ce);
void compile_expr(CompilationEngine *ce);
void compile_term(CompilationEngine *ce);
int compile_expr_list(CompilationEngine *ce);

#endif
