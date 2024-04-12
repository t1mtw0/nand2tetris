#include "common.h"
#include "compilation_engine.h"
#include "tokenizer.h"
#include <string.h>

void run(struct Tokenizer t, struct CompilationEngine ce) {
    struct Token tok;
    while ((tok = scan_token(&t)).type != T_EOF) {
        if (tok.type == T_IDENTIFIER)
            printf("T_IDENTIFIER\n");
        else if (tok.type == T_INT_CONST)
            printf("T_INT_CONST\n");
        else if (tok.type == T_STRING_CONST)
            printf("T_STRING_CONST\n");
        else if (tok.type == T_KEYWORD)
            printf("T_KEYWORD\n");
        else if (tok.type == T_SYMBOL)
            printf("T_SYMBOL\n");
    }
}

char *get_prog_name(char jack_file[]) {
    int jack_file_l = strlen(jack_file);
    char *prog_name = malloc(sizeof(char) * (jack_file_l - 4));
    strncpy(prog_name, jack_file, jack_file_l - 5);
    prog_name[jack_file_l - 5] = '\0';
    return prog_name;
}

char *append_vm_ext(char prog_name[]) {
    int prog_name_l = strlen(prog_name);
    char *fon = malloc(sizeof(char) * (prog_name_l + 4));
    strncpy(fon, prog_name, prog_name_l);
    strncpy(fon + prog_name_l, ".vm", 4);
    return fon;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stdout, "%s\n", "Usage: jack_analyzer [FILE_NAME].jack");
        exit(1);
    }
    bool is_jack_file = strlen(argv[1]) > 5 &&
                        !strncmp(argv[1] + strlen(argv[1]) - 5, ".jack", 5);
    if (!is_jack_file) {
        fprintf(stdout, "%s\n", "Usage: jack_analyzer [FILE_NAME].jack");
        exit(1);
    }

    char *prog_name = get_prog_name(argv[1]);
    char *fon = append_vm_ext(prog_name);

    FILE *fip = fopen(argv[1], "r");
    FILE *fop = fopen(fon, "w");

    struct Tokenizer t = new_tokenizer(fip);
    struct CompilationEngine ce = new_compilation_engine(fip, fop);

    run(t, ce);

    free(prog_name);
    free(fon);

    return 0;
}
