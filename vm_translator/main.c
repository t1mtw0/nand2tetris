#include "common.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    if (argc != 2)
        exit(1);
    char *fn = argv[1];

    FILE *fp = fopen(fn, "r");
    struct Parser p = new_parser(fp);

    while (advance(&p)) {
        if (p.command_type == C_ARITHMETIC) {
            printf("C_ARITHMETIC\n");
        } else if (p.command_type == C_PUSH) {
            printf("C_PUSH\n");
        } else if (p.command_type == C_POP) {
            printf("C_POP\n");
        }
        if (p.arg1 != NULL)
            printf("%s\n", p.arg1);
        if (p.arg2 != NULL)
            printf("%s\n", p.arg2);
    }

    fclose(fp);
    free_parser(&p);
}
