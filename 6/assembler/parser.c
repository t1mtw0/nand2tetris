#include "parser.h"
#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Parser newParser(FILE *fp) {
    struct Parser p = {fp, true, A_INSTRUCTION, NULL, NULL, NULL, NULL};
    return p;
}

bool advance(struct Parser *parser) {
    if (!parser->hasMoreLines)
        return false;
    char *l = NULL;
    char *line = NULL;
    size_t read;
    size_t len;
    if ((read = getline(&l, &len, parser->fp)) == -1) {
        parser->hasMoreLines = false;
        free(l);
        return false;
    }
    line = l;
    if (line[0] == '/' && line[1] == '/') {
        free(l);
        return advance(parser);
    }
    while (isspace((unsigned char)*line)) {
        line++;
        read--;
    }
    char *end = line + read - 1;
    while (end > line && isspace((unsigned char)*end)) {
        end--;
        read--;
    }
    end[1] = '\0';
    if (line[0] == '\0') {
        free(l);
        return advance(parser);
    }
    if (parser->sym != NULL) {
        free(parser->sym);
        parser->sym = NULL;
    }
    if (parser->comp != NULL) {
        free(parser->comp);
        parser->comp = NULL;
    }
    if (parser->dest != NULL) {
        free(parser->dest);
        parser->dest = NULL;
    }
    if (parser->jump != NULL) {
        free(parser->jump);
        parser->jump = NULL;
    }
    if (line[0] == '@') {
        parser->type = A_INSTRUCTION;
        char *sym = malloc(sizeof(*line) * (read - 1));
        strncpy(sym, line + 1, read - 1);
        parser->sym = sym;
    } else if (line[0] == '(') {
        parser->type = L_INSTRUCTION;
        char *sym = malloc(sizeof(*line) * (read - 2));
        strncpy(sym, line + 1, read - 2);
        parser->sym = sym;
    } else {
        parser->type = C_INSTRUCTION;
        int eqSpot = -1;
        int semiSpot = -1;
        for (int i = 0; i < read; i++) {
            if (line[i] == '=')
                eqSpot = i;
            if (line[i] == ';')
                semiSpot = i;
        }
        if (eqSpot == -1 && semiSpot == -1) {
            char *comp = malloc(sizeof(*line) * (read));
            strncpy(comp, line, read);
            parser->comp = comp;
        } else if (eqSpot == -1 && semiSpot != -1) {
            char *comp = malloc(sizeof(*line) * semiSpot);
            strncpy(comp, line, semiSpot);
            parser->comp = comp;
            char *jump = malloc(sizeof(*line) * (read - (semiSpot + 1)));
            strncpy(jump, line + semiSpot + 1, read - (semiSpot + 1));
            parser->jump = jump;
        } else if (eqSpot != -1 && semiSpot == -1) {
            char *dest = malloc(sizeof(*line) * eqSpot);
            strncpy(dest, line, eqSpot);
            parser->dest = dest;
            char *comp = malloc(sizeof(*line) * (read - (eqSpot + 1)));
            strncpy(comp, line + eqSpot + 1, read - (eqSpot + 1));
            parser->comp = comp;
        } else {
            char *dest = malloc(sizeof(*line) * eqSpot);
            strncpy(dest, line, eqSpot);
            parser->dest = dest;
            char *comp = malloc(sizeof(*line) * (semiSpot - (eqSpot + 1)));
            strncpy(comp, line + eqSpot + 1, read - (eqSpot + 1));
            parser->comp = comp;
            char *jump = malloc(sizeof(*line) * (read - (semiSpot + 1)));
            strncpy(jump, line + semiSpot + 1, read - (semiSpot + 1));
            parser->jump = jump;
        }
    }
    free(l);
    return true;
}

enum InstrType instructionType(struct Parser *parser) { return parser->type; }

char *symbol(struct Parser *parser) {
    if (parser->type != A_INSTRUCTION && parser->type != L_INSTRUCTION)
        return NULL;
    return parser->sym;
}

char *dest(struct Parser *parser) {
    if (parser->type != C_INSTRUCTION)
        return NULL;
    return parser->dest;
}

char *comp(struct Parser *parser) {
    if (parser->type != C_INSTRUCTION)
        return NULL;
    return parser->comp;
}

char *jump(struct Parser *parser) {
    if (parser->type != C_INSTRUCTION)
        return NULL;
    return parser->jump;
}

void freeParser(struct Parser *parser) {
    if (parser->sym != NULL)
        free(parser->sym);
    if (parser->dest != NULL)
        free(parser->dest);
    if (parser->comp != NULL)
        free(parser->comp);
    if (parser->jump != NULL)
        free(parser->jump);
}
