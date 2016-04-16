#include <stdio.h>
#include <string.h>

#include "type.h"
#include "st.h"

#define NR_ST 40

typedef struct {
    enum { VARIABLE, FUNCTION } kind;
    char *name;
    union {
        struct {
            Type *type;
        } variable;
        struct {
            Type *returnType;
            void *paramList;
        } function;
    };
} Symbol;

static Symbol st[NR_ST];
static int top = 1; // reserve st[0]

void install_variable(char *text, Type *type) {
    printf("install variable '%s'\n", text);
    st[top].kind = VARIABLE;
    st[top].name = text;
    st[top].variable.type = type;
    top++;
}

int contains_symbol(char *name) {
    for (int i = 1; i < top; i++) {
        if (strcmp(st[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

int contains_variable(char *name) {
    for (int i = 1; i < top; i++) {
        if (st[i].kind == VARIABLE 
                && strcmp(st[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

int contains_function(char *name) {
    for (int i = 1; i < top; i++) {
        if (st[i].kind == FUNCTION 
                && strcmp(st[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

void print_symbol_table() {
    for (int i = 1; i < top; i++) {
        printf("[%d]", i);
        switch (st[i].kind) {
        case VARIABLE:
            printf(" (variable) ");
            break;
        case FUNCTION:
            printf(" (function) ");
            break;
        default:
            printf("\nFatal: unknown symbol kind\n");
        }
        printf("%s", st[i].name);
        printf("\n");
    }
}
