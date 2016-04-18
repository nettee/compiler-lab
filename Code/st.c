#include <stdio.h>
#include <string.h>

#include "common.h"
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
            TypeNode *paramTypeList;
        } function;
    };
} Symbol;

static Symbol st[NR_ST];
static int top = 1; // reserve st[0]

void install_variable(char *text, Type *type) {
    if (contains_variable(text)) {
        return;
    }
    st[top].kind = VARIABLE;
    st[top].name = text;
    st[top].variable.type = type;
    top++;
    info("install variable '%s'", text);
}

void install_function(char *name, Type *returnType, TypeNode *paramTypeList) {
    if (contains_function(name)) {
        return;
    }
    st[top].kind = FUNCTION;
    st[top].name = name;
    st[top].function.returnType = returnType;
    st[top].function.paramTypeList = paramTypeList;
    top++;
    info("install function '%s'", name);
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

Type *retrieve_variable_type(char *name) {
    for (int i = 1; i < top; i++) {
        if (st[i].kind == VARIABLE
                && strcmp(st[i].name, name) == 0) {
            return st[i].variable.type;
        }
    }
    fatal("cannot retrieve variable '%s'", name);
}

Type *retrieve_function_returnType(char *name) {
    for (int i = 1; i < top; i++) {
        if (st[i].kind == FUNCTION
                && strcmp(st[i].name, name) == 0) {
            return st[i].function.returnType;
        }
    }
    fatal("cannot retrieve function '%s'", name);
}

TypeNode *retrieve_function_paramTypeList(char *name) {
    for (int i = 1; i < top; i++) {
        if (st[i].kind == FUNCTION
                && strcmp(st[i].name, name) == 0) {
            return st[i].function.paramTypeList;
        }
    }
    fatal("cannot retrieve function '%s'", name);
}

void print_symbol_table() {
    for (int i = 1; i < top; i++) {
        printf("[%d]", i);
        if (st[i].kind == VARIABLE) {
            printf(" (variable) ");
            printf("%s", st[i].name);
            printf(" : ");
            printf("%s", typeRepr(st[i].variable.type));
        } else if (st[i].kind == FUNCTION) {
            printf(" (function) ");
            printf("%s", st[i].name);
            printf(" : ");
            printf("%s", typeListRepr(st[i].function.paramTypeList));
            printf(" -> %s", typeRepr(st[i].function.returnType));
        } else {
            fatal("unknown symbol kind");
        }
        printf("\n");
    }
}
