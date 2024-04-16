#include "common.h"
#include "compilation_engine.h"
#include "tokenizer.h"
#include <string.h>

void run(struct CompilationEngine *ce) { compile(ce); }

void create_tok_xml(struct Tokenizer t, struct CompilationEngine ce,
                    char *xml_file) {
    FILE *fp = fopen(xml_file, "w");
    fputs("<tokens>\n", fp);

    char *buf = NULL;
    struct Token tok;
    while ((tok = scan_token(&t)).type != T_EOF) {
        if (!buf)
            buf = malloc(sizeof(char) * (tok.length + 1));
        else
            buf = realloc(buf, sizeof(char) * (tok.length + 1));

        if (tok.type == T_IDENTIFIER) {
            fputs("<identifier> ", fp);
            strncpy(buf, tok.begin, tok.length);
            buf[tok.length] = '\0';
            fputs(buf, fp);
            fputs(" </identifier>\n", fp);
        } else if (tok.type == T_INT_CONST) {
            fputs("<integerConstant> ", fp);
            strncpy(buf, tok.begin, tok.length);
            buf[tok.length] = '\0';
            fputs(buf, fp);
            fputs(" </integerConstant>\n", fp);
        } else if (tok.type == T_STRING_CONST) {
            fputs("<stringConstant> ", fp);
            strncpy(buf, tok.begin, tok.length);
            buf[tok.length] = '\0';
            fputs(buf, fp);
            fputs(" </stringConstant>\n", fp);
        } else if (tok.type == T_KEYWORD) {
            fputs("<keyword> ", fp);
            strncpy(buf, tok.begin, tok.length);
            buf[tok.length] = '\0';
            fputs(buf, fp);
            fputs(" </keyword>\n", fp);
        } else if (tok.type == T_SYMBOL) {
            fputs("<symbol> ", fp);
            strncpy(buf, tok.begin, tok.length);
            buf[tok.length] = '\0';
            if (!strncmp(buf, "<", 1))
                fputs("&lt;", fp);
            else if (!strncmp(buf, ">", 1))
                fputs("&gt;", fp);
            else if (!strncmp(buf, "&", 1))
                fputs("&amp;", fp);
            else
                fputs(buf, fp);
            fputs(" </symbol>\n", fp);
        }
    }

    fputs("</tokens>\n", fp);
    free(buf);
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

char *append_tokens_ext(char prog_name[]) {
    int prog_name_l = strlen(prog_name);
    char *fon = malloc(sizeof(char) * (prog_name_l + 12));
    strncpy(fon, prog_name, prog_name_l);
    strncpy(fon + prog_name_l, "_tokens.xml", 12);
    return fon;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stdout, "%s\n", "Usage: jack_compiler [FILE_NAME].jack");
        exit(1);
    }
    bool is_jack_file = strlen(argv[1]) > 5 &&
                        !strncmp(argv[1] + strlen(argv[1]) - 5, ".jack", 5);
    if (!is_jack_file) {
        fprintf(stdout, "%s\n", "Usage: jack_compiler [FILE_NAME].jack");
        exit(1);
    }

    char *prog_name = get_prog_name(argv[1]);
    char *fon = append_vm_ext(prog_name);
    char *ton = append_tokens_ext(prog_name);

    FILE *fip = fopen(argv[1], "r");
    FILE *fop = fopen(fon, "w");

    struct Tokenizer t = new_tokenizer(fip);
    struct CompilationEngine ce = new_compilation_engine(&t, fop);

    create_tok_xml(t, ce, ton);
    run(&ce);

    free(prog_name);
    free(fon);
    free(ton);

    return 0;
}
