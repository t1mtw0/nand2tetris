#include "compilation_engine.h"

struct CompilationEngine new_compilation_engine(FILE *fip, FILE *fop) {
    struct CompilationEngine ce = {.fip = fip, .fop = fop};
    return ce;
}

void compile_class(struct CompilationEngine *ce) {

}

void compile_class_var_dec(struct CompilationEngine *ce) {

}


void compile_subroutine(struct CompilationEngine *ce) {

}

void compile_par_list(struct CompilationEngine *ce) {

}

void compile_subroutine_body(struct CompilationEngine *ce) {

}

void compile_var_dec(struct CompilationEngine *ce) {

}

void compile_stmts(struct CompilationEngine *ce) {

}

void compile_let(struct CompilationEngine *ce) {

}

void compile_if(struct CompilationEngine *ce) {

}

void compile_while(struct CompilationEngine *ce) {

}

void compile_do(struct CompilationEngine *ce) {

}

void compile_return(struct CompilationEngine *ce) {

}

void compile_expr(struct CompilationEngine *ce) {

}

void compile_term(struct CompilationEngine *ce) {

}

int compile_expr_list(struct CompilationEngine *ce) {

}
