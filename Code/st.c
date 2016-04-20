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

typedef struct {
    Symbol st[NR_ST];
    int top;
} Env;

static Env env0;
static Env *cenv = &env0;

void init_env() {
    env0.top = 1;
}

void install_variable(char *text, Type *type) {
    Symbol *st = cenv->st;
    if (contains_variable(text)) {
        return;
    }
    st[cenv->top].kind = VARIABLE;
    st[cenv->top].name = text;
    st[cenv->top].variable.type = type;
    cenv->top++;
    info("install variable '%s'", text);
}

void install_function(char *name, Type *returnType, TypeNode *paramTypeList) {
    Symbol *st = cenv->st;
    if (contains_function(name)) {
        return;
    }
    st[cenv->top].kind = FUNCTION;
    st[cenv->top].name = name;
    st[cenv->top].function.returnType = returnType;
    st[cenv->top].function.paramTypeList = paramTypeList;
    cenv->top++;
    info("install function '%s'", name);
}

int contains_symbol(char *name) {
    Symbol *st = cenv->st;
    for (int i = 1; i < cenv->top; i++) {
        if (strcmp(st[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

int contains_variable(char *name) {
    Symbol *st = cenv->st;
    for (int i = 1; i < cenv->top; i++) {
        if (st[i].kind == VARIABLE 
                && strcmp(st[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

int contains_function(char *name) {
    Symbol *st = cenv->st;
    for (int i = 1; i < cenv->top; i++) {
        if (st[i].kind == FUNCTION 
                && strcmp(st[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

Type *retrieve_variable_type(char *name) {
    Symbol *st = cenv->st;
    for (int i = 1; i < cenv->top; i++) {
        if (st[i].kind == VARIABLE
                && strcmp(st[i].name, name) == 0) {
            return st[i].variable.type;
        }
    }
    fatal("cannot retrieve variable '%s'", name);
}

Type *retrieve_function_returnType(char *name) {
    Symbol *st = cenv->st;
    for (int i = 1; i < cenv->top; i++) {
        if (st[i].kind == FUNCTION
                && strcmp(st[i].name, name) == 0) {
            return st[i].function.returnType;
        }
    }
    fatal("cannot retrieve function '%s'", name);
}

TypeNode *retrieve_function_paramTypeList(char *name) {
    Symbol *st = cenv->st;
    for (int i = 1; i < cenv->top; i++) {
        if (st[i].kind == FUNCTION
                && strcmp(st[i].name, name) == 0) {
            return st[i].function.paramTypeList;
        }
    }
    fatal("cannot retrieve function '%s'", name);
}

void print_symbol_table() {
    Symbol *st = cenv->st;
    for (int i = 1; i < cenv->top; i++) {
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
