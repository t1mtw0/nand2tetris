#include "code_writer.h"
#include "common.h"
#include "parser.h"
#include <string.h>

// helper functions

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

void write_bootstrap(struct CodeWriter *cw) {
    fputs("@256\n", cw->fp);
    fputs("D=A\n", cw->fp);
    fputs("@SP\n", cw->fp);
    fputs("M=D\n", cw->fp);
    fputs("@Sys.init\n", cw->fp);
    fputs("0;JMP\n", cw->fp);
}

// code writer functions

struct CodeWriter new_code_writer(char *proj_dir, char *prog_name) {
    int proj_dir_l = strlen(proj_dir);
    int prog_name_l = strlen(prog_name);
    char *fn = malloc(proj_dir_l + prog_name_l + 6);
    strncpy(fn, proj_dir, strlen(proj_dir));
    strncpy(fn + proj_dir_l, "/", 1);
    strncpy(fn + proj_dir_l + 1, prog_name, prog_name_l);
    strncpy(fn + proj_dir_l + prog_name_l + 1, ".asm", 5);
    FILE *fp = fopen(fn, "w");
    struct CodeWriter cw = {.fn = fn,
                            .fp = fp,
                            .file_name = NULL,
                            .cur_fn = "Main.main",
                            .jmp_label = 0,
                            .fn_ret_label = 0};
    write_bootstrap(&cw);
    return cw;
}

void set_file_name(struct CodeWriter *cw, char *file_name) {
    cw->file_name = file_name;
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
            snprintf(s1, 100, "@%s.%d\n", cw->file_name, index);
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
            snprintf(s1, 100, "@%s.%d\n", cw->file_name, index);
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

void write_label(struct CodeWriter *cw, char *label) {
    if (!cw->cur_fn) {
        end_code_writer(cw);
        fprintf(stderr, "%s\n", "Labels can only be inside functions.");
        exit(1);
    }
    char s1[100];
    snprintf(s1, 100, "(%s$%s)\n", cw->cur_fn, label);
    fputs(s1, cw->fp);
}

void write_goto(struct CodeWriter *cw, char *label) {
    if (!cw->cur_fn) {
        end_code_writer(cw);
        fprintf(stderr, "%s\n", "Gotos can only be inside functions.");
        exit(1);
    }
    char s1[100];
    snprintf(s1, 100, "@%s$%s\n", cw->cur_fn, label);
    fputs(s1, cw->fp);
    fputs("0;JMP\n", cw->fp);
}

void write_if(struct CodeWriter *cw, char *label) {
    if (!cw->cur_fn) {
        end_code_writer(cw);
        fprintf(stderr, "%s\n", "Gotos can only be inside functions.");
        exit(1);
    }
    pop_and_address(cw);
    fputs("D=M\n", cw->fp);
    char s1[10];
    snprintf(s1, 10, "@j%dcont\n", cw->jmp_label);
    fputs(s1, cw->fp);
    fputs("D;JEQ\n", cw->fp);
    char s2[100];
    snprintf(s2, 100, "@%s$%s\n", cw->cur_fn, label);
    fputs(s2, cw->fp);
    fputs("0;JMP\n", cw->fp);
    char s3[10];
    snprintf(s3, 10, "(j%dcont)\n", cw->jmp_label);
    fputs(s3, cw->fp);
    cw->jmp_label++;
}

void write_function(struct CodeWriter *cw, char *function_name, int n_vars) {
    cw->cur_fn = function_name;
    char s1[100];
    snprintf(s1, 100, "(%s)\n", function_name);
    fputs(s1, cw->fp);
    if (n_vars != 0)
        address_stack(cw);
    for (int i = 0; i < n_vars; i++) {
        fputs("M=0\n", cw->fp);
        fputs("@SP\n", cw->fp);
        fputs("AM=M+1\n", cw->fp);
    }
}

void write_call(struct CodeWriter *cw, char *function_name, int n_args) {
    // push return address
    char s1[100];
    snprintf(s1, 100, "@%s$ret%d\n", cw->cur_fn, cw->fn_ret_label);
    fputs(s1, cw->fp);
    fputs("D=A\n", cw->fp);
    address_stack(cw);
    fputs("M=D\n", cw->fp);
    fputs("@SP\n", cw->fp);
    fputs("M=M+1\n", cw->fp);
    // push local
    fputs("@LCL\n", cw->fp);
    fputs("D=M\n", cw->fp);
    address_stack(cw);
    fputs("M=D\n", cw->fp);
    fputs("@SP\n", cw->fp);
    fputs("M=M+1\n", cw->fp);
    // push argument
    fputs("@ARG\n", cw->fp);
    fputs("D=M\n", cw->fp);
    address_stack(cw);
    fputs("M=D\n", cw->fp);
    fputs("@SP\n", cw->fp);
    fputs("M=M+1\n", cw->fp);
    // push this
    fputs("@THIS\n", cw->fp);
    fputs("D=M\n", cw->fp);
    address_stack(cw);
    fputs("M=D\n", cw->fp);
    fputs("@SP\n", cw->fp);
    fputs("M=M+1\n", cw->fp);
    // push that
    fputs("@THAT\n", cw->fp);
    fputs("D=M\n", cw->fp);
    address_stack(cw);
    fputs("M=D\n", cw->fp);
    fputs("@SP\n", cw->fp);
    fputs("M=M+1\n", cw->fp);
    // ARG = SP - 5 - n_args
    fputs("@5\n", cw->fp);
    fputs("D=A\n", cw->fp);
    char s2[10];
    snprintf(s2, 10, "@%d\n", n_args);
    fputs(s2, cw->fp);
    fputs("D=D+A\n", cw->fp);
    fputs("@SP\n", cw->fp);
    fputs("D=M-D\n", cw->fp);
    fputs("@ARG\n", cw->fp);
    fputs("M=D\n", cw->fp);
    // LCL = SP
    fputs("@SP\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@LCL\n", cw->fp);
    fputs("M=D\n", cw->fp);
    // goto function_name
    char s3[100];
    snprintf(s3, 100, "@%s\n", function_name);
    fputs(s3, cw->fp);
    fputs("0;JMP\n", cw->fp);
    char s4[100];
    snprintf(s4, 100, "(%s$ret%d)\n", cw->cur_fn, cw->fn_ret_label);
    fputs(s4, cw->fp);
    cw->fn_ret_label++;
}

void write_return(struct CodeWriter *cw) {
    fputs("@LCL\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@R14\n", cw->fp);
    fputs("M=D\n", cw->fp);
    fputs("@5\n", cw->fp);
    fputs("A=D-A\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@R15\n", cw->fp);
    fputs("M=D\n", cw->fp);
    pop_and_address(cw);
    fputs("D=M\n", cw->fp);
    fputs("@ARG\n", cw->fp);
    fputs("A=M\n", cw->fp);
    fputs("M=D\n", cw->fp);
    fputs("@ARG\n", cw->fp);
    fputs("D=M+1\n", cw->fp);
    fputs("@SP\n", cw->fp);
    fputs("M=D\n", cw->fp);
    // restore THAT
    fputs("@R14\n", cw->fp);
    fputs("A=M-1\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@THAT\n", cw->fp);
    fputs("M=D\n", cw->fp);
    // restore THIS
    fputs("@R14\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@2\n", cw->fp);
    fputs("A=D-A\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@THIS\n", cw->fp);
    fputs("M=D\n", cw->fp);
    // restore ARG
    fputs("@R14\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@3\n", cw->fp);
    fputs("A=D-A\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@ARG\n", cw->fp);
    fputs("M=D\n", cw->fp);
    // restore LCL
    fputs("@R14\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@4\n", cw->fp);
    fputs("A=D-A\n", cw->fp);
    fputs("D=M\n", cw->fp);
    fputs("@LCL\n", cw->fp);
    fputs("M=D\n", cw->fp);
    // goto return address
    fputs("@R15\n", cw->fp);
    fputs("A=M\n", cw->fp);
    fputs("0;JMP\n", cw->fp);
}

void end_code_writer(struct CodeWriter *cw) {
    fclose(cw->fp);
    free(cw->fn);
}
