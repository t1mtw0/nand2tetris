#include "code_writer.h"
#include "common.h"
#include "parser.h"
#include <string.h>

char *map_segment(char *segment) {
    if (strcmp(segment, "argument") == 0) {
        return "ARG";
    } else if (strcmp(segment, "local") == 0) {
        return "LCL";
    } else if (strcmp(segment, "this") == 0) {
        return "THIS";
    } else if (strcmp(segment, "that") == 0) {
        return "THAT";
    } else if (strcmp(segment, "temp") == 0) {
        return "TEMP";
    }
    return NULL;
}

void pop_and_address(struct CodeWriter *cw) {
    fputs("@SP\n", cw->fp);
    fputs("AM=M-1\n", cw->fp);
}

void address_stack(struct CodeWriter *cw) {
    fputs("@SP\n", cw->fp);
    fputs("A=M\n", cw->fp);
}

void write_cmp_op(struct CodeWriter *cw, char *cmp) {
    fputs("D=D-M\n", cw->fp);
    char s1[13];
    snprintf(s1, 13, "@j%dsuccess\n", cw->jmp_label);
    fputs(s1, cw->fp);
    char s2[7];
    snprintf(s2, 7, "D;%s\n", cmp);
    fputs(s2, cw->fp);
    fputs("@R13\n", cw->fp);
    fputs("A=M\n", cw->fp);
    fputs("M=0\n", cw->fp);
    char s3[8];
    snprintf(s3, 8, "@j%dend\n", cw->jmp_label);
    fputs(s3, cw->fp);
    fputs("0;JMP\n", cw->fp);
    char s4[14];
    snprintf(s4, 14, "(j%dsuccess)\n", cw->jmp_label);
    fputs(s4, cw->fp);
    fputs("@R13\n", cw->fp);
    fputs("A=M\n", cw->fp);
    fputs("M=-1\n", cw->fp);
    char s5[10];
    snprintf(s5, 10, "(j%dend)\n", cw->jmp_label);
    fputs(s5, cw->fp);
    cw->jmp_label++;
}

struct CodeWriter new_code_writer(char *prog_name, FILE *fp) {
    struct CodeWriter cw = {.prog_name = prog_name, .fp = fp, .jmp_label = 0};
    return cw;
}

void write_arithmetic(struct CodeWriter *cw, char *command) {
    // unary operation or not
    if (strcmp(command, "neg") == 0 || strcmp(command, "not") == 0) {
        address_stack(cw);
        fputs("A=A-1\n", cw->fp);
    } else if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0 ||
               strcmp(command, "and") == 0 || strcmp(command, "or") == 0) {
        pop_and_address(cw);
        fputs("D=M\n", cw->fp);
        fputs("A=A-1\n", cw->fp);
    } else if (strcmp(command, "eq") == 0 || strcmp(command, "gt") == 0 ||
               strcmp(command, "lt") == 0) {
        pop_and_address(cw);
        fputs("A=A-1\n", cw->fp);
        fputs("D=A\n", cw->fp);
        fputs("@R13\n", cw->fp);
        fputs("AM=D\n", cw->fp);
        fputs("D=M\n", cw->fp);
        fputs("A=A+1\n", cw->fp);
    }
    if (strcmp(command, "add") == 0) {
        fputs("M=D+M\n", cw->fp);
    } else if (strcmp(command, "sub") == 0) {
        fputs("M=M-D\n", cw->fp);
    } else if (strcmp(command, "neg") == 0) {
        fputs("M=-M\n", cw->fp);
    } else if (strcmp(command, "eq") == 0) {
        write_cmp_op(cw, "JEQ");
    } else if (strcmp(command, "gt") == 0) {
        write_cmp_op(cw, "JGT");
    } else if (strcmp(command, "lt") == 0) {
        write_cmp_op(cw, "JLT");
    } else if (strcmp(command, "and") == 0) {
        fputs("M=D&M\n", cw->fp);
    } else if (strcmp(command, "or") == 0) {
        fputs("M=D|M\n", cw->fp);
    } else if (strcmp(command, "not") == 0) {
        fputs("M=!M\n", cw->fp);
    }
}

void write_push_pop(struct CodeWriter *cw, enum CommandType command,
                    char *segment, int index) {
    if (command == C_PUSH) {
        // put correct segment value into register D
        if (strcmp(segment, "local") == 0 || strcmp(segment, "argument") == 0 ||
            strcmp(segment, "this") == 0 || strcmp(segment, "that") == 0) {
            char *seg_n = map_segment(segment);
            char s1[10];
            snprintf(s1, 10, "@%s\n", seg_n);
            fputs(s1, cw->fp);
            fputs("D=M\n", cw->fp);
            char s2[10];
            snprintf(s2, 10, "@%d\n", index);
            fputs(s2, cw->fp);
            fputs("A=A+D\n", cw->fp);
            fputs("D=M\n", cw->fp);
        } else if (strcmp(segment, "static") == 0) {
            char s1[100];
            snprintf(s1, 100, "@%s.%d\n", cw->prog_name, index);
            fputs(s1, cw->fp);
            fputs("D=M\n", cw->fp);
        } else if (strcmp(segment, "constant") == 0) {
            char s1[10];
            snprintf(s1, 10, "@%d\n", index);
            fputs(s1, cw->fp);
            fputs("D=A\n", cw->fp);
        } else if (strcmp(segment, "pointer") == 0) {
            if (index != 0 && index != 1)
                exit(1);
            if (index == 0)
                fputs("@THIS\n", cw->fp);
            else
                fputs("@THAT\n", cw->fp);
            fputs("D=M\n", cw->fp);
        } else if (strcmp(segment, "temp") == 0) {
            if (index < 0 || index > 7)
                exit(1);
            char s1[10];
            snprintf(s1, 10, "@%d\n", index);
            fputs(s1, cw->fp);
            fputs("D=A\n", cw->fp);
            fputs("@5\n", cw->fp);
            fputs("A=A+D\n", cw->fp);
            fputs("D=M\n", cw->fp);
        }
        // push contents of register D onto stack
        address_stack(cw);
        fputs("M=D\n", cw->fp);
        fputs("@SP\n", cw->fp);
        fputs("M=M+1\n", cw->fp);
    } else if (command == C_POP) {
        if (strcmp(segment, "local") == 0 || strcmp(segment, "argument") == 0 ||
            strcmp(segment, "this") == 0 || strcmp(segment, "that") == 0) {
            char *seg_n = map_segment(segment);
            char s1[10];
            snprintf(s1, 10, "@%s\n", seg_n);
            fputs(s1, cw->fp);
            fputs("D=M\n", cw->fp);
            char s2[10];
            snprintf(s2, 10, "@%d\n", index);
            fputs(s2, cw->fp);
            fputs("D=A+D\n", cw->fp);
            fputs("@R13\n", cw->fp);
            fputs("M=D\n", cw->fp);
            pop_and_address(cw);
            fputs("D=M\n", cw->fp);
            fputs("@R13\n", cw->fp);
            fputs("A=M\n", cw->fp);
            fputs("M=D\n", cw->fp);
        } else if (strcmp(segment, "static") == 0) {
            pop_and_address(cw);
            fputs("D=M\n", cw->fp);
            char s1[100];
            snprintf(s1, 100, "@%s.%d\n", cw->prog_name, index);
            fputs(s1, cw->fp);
            fputs("M=D\n", cw->fp);
        } else if (strcmp(segment, "pointer") == 0) {
            pop_and_address(cw);
            fputs("D=M\n", cw->fp);
            if (index != 0 && index != 1)
                exit(1);
            if (index == 0)
                fputs("@THIS\n", cw->fp);
            else
                fputs("@THAT\n", cw->fp);
            fputs("M=D\n", cw->fp);
        } else if (strcmp(segment, "temp") == 0) {
            if (index < 0 || index > 7)
                exit(1);
            char s1[10];
            snprintf(s1, 10, "@%d\n", index);
            fputs(s1, cw->fp);
            fputs("D=A\n", cw->fp);
            fputs("@5\n", cw->fp);
            fputs("D=A+D\n", cw->fp);
            fputs("@R13\n", cw->fp);
            fputs("M=D\n", cw->fp);
            pop_and_address(cw);
            fputs("D=M\n", cw->fp);
            fputs("@R13\n", cw->fp);
            fputs("A=M\n", cw->fp);
            fputs("M=D\n", cw->fp);
        }
    }
}

void end_code_writer(struct CodeWriter *cw) { fclose(cw->fp); }
