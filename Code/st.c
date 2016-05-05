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
    if (!isStructureType(type)) {
        warn("installing non-struct type '%s'", typeRepr(type));
        return;
    }
    if (type->structure.name == NULL) {
        info("discard '%s' with no name", typeRepr(type));
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
    if (name == NULL) {
        // anonymous struct
        return false;
    }
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
    char *name;
    Type *type;
} Variable;

typedef struct VarNode_ VarNode;

struct VarNode_ {
    VarNode *next;
    Variable *var;
};

typedef struct {
    enum { DECLARED, DEFINED } state;
    int lineno;
    Type *returnType;
    TypeNode *paramTypeList;
} Function;

typedef struct FunNode_ FunNode;

struct FunNode_ {
    FunNode *next;
    Function *fun;
};

typedef struct Env_ {
    struct Env_ *next;
    VarNode *vst; // variable symbol table
    FunNode *fst; // function symbol table
} Env;

static Env *cenv;

bool in_nested_env() {
    return cenv->next != NULL;
}

void init_env() {
    cenv = malloc(sizeof(Env));
    cenv->next = NULL;
    cenv->vtop = 1;
    cenv->ftop = 1;
}

void enter_new_env() {
    Env *env = malloc(sizeof(Env));
    env->next = cenv;
    env->vtop = 1;
    env->ftop = 1;
    cenv = env;
}

FieldNode *generateFieldList(Env *cenv) {
    FieldNode *head = NULL;
    FieldNode *tail = NULL;
    Symbol *st = cenv->vst;
    for (int i = 1; i < cenv->vtop; i++) {
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
    Symbol *st = cenv->vst;
    if (contains_variable(text)) {
        return;
    }
    st[cenv->vtop].name = text;
    st[cenv->vtop].variable.type = type;
    cenv->vtop++;
    info("install variable '%s'", text);
}

void install_function_defined(char *name, Type *returnType, TypeNode *paramTypeList) {
    Symbol *st = cenv->fst;
    if (contains_function_defined(name)) {
        return;
    }
    if (contains_function_declared(name)) {
        for (int i = 1; i < cenv->ftop; i++) {
            if (strcmp(st[i].name, name) == 0
                    && st[i].function.state == DECLARED) {
                st[i].function.state = DEFINED;
                info("upgrade function '%s' to defined", name);
                return;
            }
        }
    }

    st[cenv->ftop].name = name;
    st[cenv->ftop].function.state = DEFINED;
    st[cenv->ftop].function.lineno = 233;
    st[cenv->ftop].function.returnType = returnType;
    st[cenv->ftop].function.paramTypeList = paramTypeList;
    cenv->ftop++;
    info("install function '%s'", name);
}

void install_function_declared(char *name, Type *returnType, 
        TypeNode *paramTypeList, int lineno) {
    Symbol *st = cenv->fst;
    if (contains_function_defined(name)
            || contains_function_declared(name)) {
        return;
    }
    st[cenv->ftop].name = name;
    st[cenv->ftop].function.state = DECLARED;
    st[cenv->ftop].function.lineno = lineno;
    st[cenv->ftop].function.returnType = returnType;
    st[cenv->ftop].function.paramTypeList = paramTypeList;
    cenv->ftop++;
    info("install function '%s'", name);
}

int contains_symbol(char *name) {
    return contains_variable(name)
            || contains_function_defined(name)
            || contains_function_declared(name);
}

int contains_variable(char *name) {
    Symbol *st = cenv->vst;
    for (int i = 1; i < cenv->vtop; i++) {
        if (strcmp(st[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

int contains_function_defined(char *name) {
    Symbol *st = cenv->fst;
    for (int i = 1; i < cenv->ftop; i++) {
        if (strcmp(st[i].name, name) == 0
                && st[i].function.state == DEFINED) {
            return 1;
        }
    }
    return 0;
}

int contains_function_declared(char *name) {
    Symbol *st = cenv->fst;
    for (int i = 1; i < cenv->ftop; i++) {
        if (strcmp(st[i].name, name) == 0
                && st[i].function.state == DECLARED) {
            return 1;
        }
    }
    return 0;
}

Type *retrieve_variable_type(char *name) {
    Symbol *st = cenv->vst;
    for (int i = 1; i < cenv->vtop; i++) {
        if (strcmp(st[i].name, name) == 0) {
            return st[i].variable.type;
        }
    }
    warn("cannot retrieve variable '%s'", name);
    return getArbitType();
}

Type *retrieve_function_returnType(char *name) {
    Symbol *st = cenv->fst;
    for (int i = 1; i < cenv->ftop; i++) {
        if (strcmp(st[i].name, name) == 0) {
            return st[i].function.returnType;
        }
    }
    warn("cannot retrieve function '%s'", name);
    return getArbitType();
}

TypeNode *retrieve_function_paramTypeList(char *name) {
    Symbol *st = cenv->fst;
    for (int i = 1; i < cenv->ftop; i++) {
        if (strcmp(st[i].name, name) == 0) {
            return st[i].function.paramTypeList;
        }
    }
    fatal("cannot retrieve function '%s'", name);
}

bool check_function_declared_undefined() {
    bool ret = true;
    Symbol *st = cenv->fst;
    for (int i = 1; i < cenv->ftop; i++) {
        if (st[i].function.state == DECLARED) {
            printf("Error type 18 at Line %d: Function '%s' declared but not defined\n", st[i].function.lineno, st[i].name);
            extern int nr_semantics_error;
            nr_semantics_error++;
        }
        ret = false;
    }
    return ret;
}


void print_symbol_table() {
    Symbol *st;
    st = cenv->vst;
    for (int i = 1; i < cenv->vtop; i++) {
        printf("[%d]", i);
        printf(" (variable) ");
        printf("%s", st[i].name);
        printf(" : ");
        printf("%s", typeRepr(st[i].variable.type));
        printf("\n");
    }

    st = cenv->fst;
    for (int i = 0; i < cenv->ftop; i++) {
        printf(" (function) ");
        if (st[i].function.state == DECLARED) {
            printf("_[%d] ", st[i].function.lineno);
        }
        printf("%s", st[i].name);
        printf(" : ");
        printf("%s", typeListRepr(st[i].function.paramTypeList));
        printf(" -> %s", typeRepr(st[i].function.returnType));
        printf("\n");
    }
}
