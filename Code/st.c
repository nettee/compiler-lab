#include <stdio.h>
#include <string.h>

#include "common.h"
#include "st.h"

#define NR_ST 40

static TypeNode *structList = NULL;

void install_struct(Type *type) {
    if (contains_struct__type(type)) {
        return;
    }
    TypeNode *typeNode = malloc(sizeof(TypeNode));
    typeNode->type = type;
    typeNode->next = structList;
    structList = typeNode;
    info("install type '%s'", typeRepr(type));
}

bool contains_struct__type(Type *type) {
    return isStructureType(type)
            && contains_struct__name(type->structure.name);
}


bool contains_struct__name(char *name) {
    for (TypeNode *q = structList; q != NULL; q = q->next) {
        if (q->type->kind == STRUCTURE
                && strcmp(q->type->structure.name, name) == 0) {
            return true;
        }
    }
    return false;
}

Type *retrieve_struct(char *name) {
    for (TypeNode *q = structList; q != NULL; q = q->next) {
        if (q->type->kind == STRUCTURE
                && strcmp(q->type->structure.name, name) == 0) {
            return q->type;
        }
    }
    warn("cannot retrieve struct '%s'", name);
    return getArbitType();
}

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

typedef struct Env_ {
    struct Env_ *next;
    Symbol st[NR_ST];
    int top;
} Env;

static Env *cenv;

bool in_nested_env() {
    return cenv->next != NULL;
}

void init_env() {
    cenv = malloc(sizeof(Env));
    cenv->next = NULL;
    cenv->top = 1;
}

void enter_new_env() {
    Env *env = malloc(sizeof(Env));
    env->next = cenv;
    env->top = 1;
    cenv = env;
}

FieldNode *generateFieldList(Env *cenv) {
    FieldNode *head = NULL;
    FieldNode *tail = NULL;
    Symbol *st = cenv->st;
    for (int i = 1; i < cenv->top; i++) {
        if (st[i].kind == VARIABLE) {
            info("name = %s, type = %s",
                    st[i].name,
                    typeRepr(st[i].variable.type));
            FieldNode *field = newFieldNode(
                    st[i].name,
                    st[i].variable.type);
            if (head == NULL) {
                head = field;
                tail = field;
            } else {
                tail->next = field;
                tail = tail->next;
            }
        }
    }
    return head;
}

FieldNode *exit_current_env() {
    FieldNode *t = generateFieldList(cenv);
    Env *temp = cenv;
    cenv = cenv->next;
    free(temp);
    return t;
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
    warn("cannot retrieve variable '%s'", name);
    return getArbitType();
}

Type *retrieve_function_returnType(char *name) {
    Symbol *st = cenv->st;
    for (int i = 1; i < cenv->top; i++) {
        if (st[i].kind == FUNCTION
                && strcmp(st[i].name, name) == 0) {
            return st[i].function.returnType;
        }
    }
    warn("cannot retrieve function '%s'", name);
    return getArbitType();
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
