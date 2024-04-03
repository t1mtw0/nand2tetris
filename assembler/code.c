#include "common.h"
#include <string.h>

char *trans_dest(char *syms) {
    if (syms == NULL)
        return NULL;
    if (strcmp(syms, "M") == 0) {
        return "001";
    } else if (strcmp(syms, "D") == 0) {
        return "010";
    } else if (strcmp(syms, "DM") == 0) {
        return "011";
    } else if (strcmp(syms, "A") == 0) {
        return "100";
    } else if (strcmp(syms, "AM") == 0) {
        return "101";
    } else if (strcmp(syms, "AD") == 0) {
        return "110";
    } else if (strcmp(syms, "ADM") == 0) {
        return "111";
    }
    return NULL;
}

char *trans_comp(char *syms) {
    if (syms == NULL)
        return NULL;
    if (strcmp(syms, "0") == 0) {
        return "0101010";
    } else if (strcmp(syms, "1") == 0) {
        return "0111111";
    } else if (strcmp(syms, "-1") == 0) {
        return "0111010";
    } else if (strcmp(syms, "D") == 0) {
        return "0001100";
    } else if (strcmp(syms, "A") == 0) {
        return "0110000";
    } else if (strcmp(syms, "!D") == 0) {
        return "0001101";
    } else if (strcmp(syms, "!A") == 0) {
        return "0110001";
    } else if (strcmp(syms, "-D") == 0) {
        return "0001111";
    } else if (strcmp(syms, "-A") == 0) {
        return "0110011";
    } else if (strcmp(syms, "D+1") == 0) {
        return "0011111";
    } else if (strcmp(syms, "A+1") == 0) {
        return "0110111";
    } else if (strcmp(syms, "D-1") == 0) {
        return "0001110";
    } else if (strcmp(syms, "A-1") == 0) {
        return "0110010";
    } else if (strcmp(syms, "D+A") == 0) {
        return "0000010";
    } else if (strcmp(syms, "D-A") == 0) {
        return "0010011";
    } else if (strcmp(syms, "A-D") == 0) {
        return "0000111";
    } else if (strcmp(syms, "D&A") == 0) {
        return "0000000";
    } else if (strcmp(syms, "D|A") == 0) {
        return "0010101";
    } else if (strcmp(syms, "M") == 0) {
        return "1110000";
    } else if (strcmp(syms, "!M") == 0) {
        return "1110001";
    } else if (strcmp(syms, "-M") == 0) {
        return "1110011";
    } else if (strcmp(syms, "M+1") == 0) {
        return "1110111";
    } else if (strcmp(syms, "M-1") == 0) {
        return "1110010";
    } else if (strcmp(syms, "D+M") == 0) {
        return "1000010";
    } else if (strcmp(syms, "D-M") == 0) {
        return "1010011";
    } else if (strcmp(syms, "M-D") == 0) {
        return "1000111";
    } else if (strcmp(syms, "D&M") == 0) {
        return "1000000";
    } else if (strcmp(syms, "D|M") == 0) {
        return "1010101";
    }
    return NULL;
}

char *trans_jump(char *syms) {
    if (syms == NULL)
        return NULL;
    if (strcmp(syms, "JGT") == 0) {
        return "001";
    } else if (strcmp(syms, "JEQ") == 0) {
        return "010";
    } else if (strcmp(syms, "JGE") == 0) {
        return "011";
    } else if (strcmp(syms, "JLT") == 0) {
        return "100";
    } else if (strcmp(syms, "JNE") == 0) {
        return "101";
    } else if (strcmp(syms, "JLE") == 0) {
        return "110";
    } else if (strcmp(syms, "JMP") == 0) {
        return "111";
    }
    return NULL;
}
