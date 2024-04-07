#include "code_writer.h"
#include "common.h"
#include "parser.h"
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2)
        exit(1);
    char *fn = argv[1];

    FILE *fp = fopen(fn, "r");

    char *s = fn;
    int i = 0;
    int dot_spot = -1;
    while (*s != '\0') {
        if (strncmp(s, ".vm", 3) == 0)
            dot_spot = i;
        i++;
        s++;
    }
    if (dot_spot == -1)
        exit(1);
    char prog_fname[dot_spot];

    strncpy(prog_fname, fn, dot_spot);
    prog_fname[strlen(prog_fname) - 1] = '\0';

    char fon[strlen(prog_fname) + 5];
    strncpy(fon, prog_fname, strlen(prog_fname));
    
    strncpy(fon + strlen(prog_fname), ".asm", 4);
    fon[strlen(fon)] = '\0';
    // extract only program name, not full path
    char *n = prog_fname + strlen(prog_fname) - 1;
    int last_slash = -1;
    int j = strlen(prog_fname) - 1;
    while (n >= prog_fname) {
        if (*n == '/') {
            last_slash = j;
            break;
        }
        j--;
        n--;
    }
    if (last_slash == -1) last_slash = 0;
    char prog_n[strlen(prog_fname) - last_slash - 1];
    strncpy(prog_n, prog_fname + last_slash + 1, strlen(prog_fname) - last_slash - 1);

    FILE *fop = fopen(fon, "w");

    struct Parser p = new_parser(fp);
    struct CodeWriter cw = new_code_writer(prog_n, fop);

    while (advance(&p)) {
        if (p.command_type == C_ARITHMETIC)
            write_arithmetic(&cw, p.arg1);
        else if (p.command_type == C_PUSH || p.command_type == C_POP)
            write_push_pop(&cw, p.command_type, p.arg1,
                           (int)strtol(p.arg2, NULL, 10));
    }

    end_parser(&p);
    end_code_writer(&cw);
}
