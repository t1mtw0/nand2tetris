#include "code_writer.h"
#include "common.h"
#include "parser.h"
#include <dirent.h>
#include <limits.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2)
        exit(1);

    char *pn_a = argv[1];

    char pn[PATH_MAX];
    char *res = realpath(pn_a, pn);
    if (!res)
        exit(1);

    int pn_l = strlen(pn);
    bool p_is_file = pn_l > 3 && !strncmp(pn + pn_l - 3, ".vm", 3);

    char **fi_li = NULL;
    int fi_li_n = 0;
    char *proj_dir;
    char *proj_name;

    if (p_is_file) {
        char *fi = strrchr(pn, '/');
        int fi_l = strlen(fi);
        fi_li_n = 1;
        // put into input file list
        fi_li = malloc(sizeof(char *));
        fi_li[0] = malloc(sizeof(char) * fi_l);
        strncpy(fi_li[0], fi + 1, fi_l);
        // extract project directory
        proj_dir = malloc(sizeof(char) * (int)(fi - pn + 1));
        strncpy(proj_dir, pn, fi - pn);
        proj_dir[fi - pn] = '\0';
        // extract project name
        char *ds = strrchr(pn, '.');
        proj_name = malloc(sizeof(char) * (int)(ds - fi));
        strncpy(proj_name, fi + 1, ds - fi - 1);
        proj_name[ds - fi] = '\0';
    } else {
        int pn_l = strlen(pn);
        // extract project directory (same as input arg)
        proj_dir = malloc(sizeof(char) * (pn_l + 1));
        strncpy(proj_dir, pn, strlen(pn) + 1);
        // extract project name
        char *ss = strrchr(pn, '/');
        proj_name = malloc(sizeof(char) * (int)(pn + pn_l - ss));
        strncpy(proj_name, ss + 1, (int)(pn + pn_l - ss));
        proj_name[pn + pn_l - ss] = '\0';
        // extract all .vm files in directory
        DIR *pd;
        struct dirent *dir;
        pd = opendir(pn);
        if (!pd)
            exit(1);
        while ((dir = readdir(pd))) {
            int dir_l = strlen(dir->d_name);
            if (!(dir_l > 3 && !strncmp(dir->d_name + dir_l - 3, ".vm", 3)))
                continue;
            if (!fi_li)
                fi_li = malloc(sizeof(char *));
            else
                fi_li = realloc(fi_li, sizeof(char *) * (fi_li_n + 1));
            fi_li[fi_li_n] = malloc(sizeof(char) * (dir_l + 1));
            strncpy(fi_li[fi_li_n], dir->d_name, dir_l + 1);
            fi_li_n++;
        }
        closedir(pd);
    }

    if (!fi_li)
        exit(1);

    struct CodeWriter cw = new_code_writer(proj_dir, proj_name);

    for (int i = 0; i < fi_li_n; i++) {
        struct Parser p = new_parser(proj_dir, fi_li[i]);
        set_file_name(&cw, fi_li[i]);
        while (advance(&p)) {
            if (p.command_type == C_ARITHMETIC)
                write_arithmetic(&cw, p.arg1);
            else if (p.command_type == C_PUSH || p.command_type == C_POP)
                write_push_pop(&cw, p.command_type, p.arg1,
                               (int)strtol(p.arg2, NULL, 10));
            else if (p.command_type == C_LABEL)
                write_label(&cw, p.arg1);
            else if (p.command_type == C_GOTO)
                write_goto(&cw, p.arg1);
            else if (p.command_type == C_IF)
                write_if(&cw, p.arg1);
            else if (p.command_type == C_FUNCTION)
                write_function(&cw, p.arg1, (int)strtol(p.arg2, NULL, 10));
            else if (p.command_type == C_CALL)
                write_call(&cw, p.arg1, (int)strtol(p.arg2, NULL, 10));
            else if (p.command_type == C_RETURN)
                write_return(&cw);
        }
        end_parser(&p);
    }

    end_code_writer(&cw);

    for (int i = 0; i < fi_li_n; i++)
        free(fi_li[i]);
    free(fi_li);
    free(proj_dir);

    return 0;
}
