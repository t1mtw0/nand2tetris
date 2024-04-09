#include "common.h"
#include "tokenizer.h"
#include "compilation_engine.h"

int main(int argc, int argv[]) {
    if (argc != 2) {
        fprintf(stderr, "%s\n", "Usage: jack_analyzer [FILE_NAME].jack");
        exit(1);
    }
    bool is_jack_file = strlen(argv[1] > 5) && !strncmp(argv[1] + strlen(argv[1]) - 5, ".jack", 5);
    if (!is_jack_file) {
        fprintf(stderr, "%s\n", "Usage: jack_analyzer [FILE_NAME].jack");
        exit(1);
    }

    char prog_name[strlen(argv[1]) - 4];
    strncpy(prog_name, argv[1], strlen(argv[1]) - 5);
    prog_name[strlen(argv[1]) - 5] = '\0';

    char fon[strlen(prog_name) + 4];
    strncpy(fon, prog_name, strlen(prog_name));
    strncpy(fon, ".vm", 4);

    printf("%s\n", prog_name);
    printf("%s\n", fon);

    FILE *fip = fopen(argv[1], "r");
    FILE *fop = fopen(fon, "w");

    struct Tokenizer t = new_tokenizer(fip);
    struct CompilationEngine ce = new_compilation_engine(fip, fop);
}
