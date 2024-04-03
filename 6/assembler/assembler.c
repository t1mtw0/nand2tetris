#include "code.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

char *dec_to_binary(long num) {
    char *bin = malloc(sizeof(char) * 15);
    int i = 14;
    while (num > 0) {
        char s;
        if (num % 2 == 0)
            s = '0';
        else
            s = '1';
        bin[i] = s;
        num = num / 2;
        i--;
    }
    for (int j = i; j >= 0; --j) {
        bin[j] = '0';
    }
    return bin;
}

int main(int argc, char *argv[]) {
    if (argc != 2)
        exit(1);
    char *fn = argv[1];

    FILE *fp = fopen(fn, "r");
    if (fp == NULL)
        exit(1);

    char *s = fn;
    int dotSpot = -1;
    int i = 0;
    while (*s != '\0') {
        if (*s == '.')
            dotSpot = i;
        s++;
        i++;
    }
    if (dotSpot == -1)
        exit(1);
    char progName[dotSpot];
    strncpy(progName, fn, dotSpot);

    char fo[dotSpot + 5];
    strncpy(fo, progName, dotSpot);
    strncpy(fo + dotSpot, ".hack", 5);

    FILE *outFp = fopen(fo, "w");

    struct Parser p = newParser(fp);

    while (advance(&p)) {
        char bitInstr[18];
        if (p.type == A_INSTRUCTION) {
            bitInstr[0] = '0';
            char *sym = dec_to_binary(strtol(p.sym, NULL, 10));
            for (int i = 1; i < 16; i++)
                bitInstr[i] = sym[i - 1];
            free(sym);
        } else if (p.type == C_INSTRUCTION) {
            bitInstr[0] = '1';
            bitInstr[1] = '1';
            bitInstr[2] = '1';
            char *bit_dest = trans_dest(p.dest);
            char *bit_comp = trans_comp(p.comp);
            char *bit_jump = trans_jump(p.jump);
            if (bit_comp == NULL)
                exit(1);
            strncpy(bitInstr + 3, bit_comp, 7);
            if (bit_dest != NULL)
                strncpy(bitInstr + 10, bit_dest, 3);
            else
                strncpy(bitInstr + 10, "000", 3);
            if (bit_jump != NULL)
                strncpy(bitInstr + 13, bit_jump, 3);
            else
                strncpy(bitInstr + 13, "000", 3);
        }
        bitInstr[16] = '\n';
        bitInstr[17] = '\0';
        fputs(bitInstr, outFp);
    }

    fclose(fp);
    fclose(outFp);
    freeParser(&p);
}
