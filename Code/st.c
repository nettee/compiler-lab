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
    union {
        // for variable
        struct {
            Type *type;
        };
        // for function
        struct {
            enum { DEFINED, DECLARED } state;
            int lineno;
            Type *returnType;
            TypeNode *paramTypeList;
        };
    };
} Symbol;

Symbol *newVariableSymbol(char *name, Type *type) {
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->name = name;
    symbol->type = type;
    return symbol;
}

Symbol *newFunctionSymbol(char *name, int lineno, Type *returnType,
        TypeNode *paramTypeList) {
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->name = name;
    symbol->lineno = lineno;
    symbol->returnType = returnType;
    symbol->paramTypeList = paramTypeList;
    return symbol;
}

typedef struct SymbolNode_ SymbolNode;

struct SymbolNode_ {
    SymbolNode *next;
    Symbol *symbol;
};

typedef struct {
    SymbolNode *head;
    SymbolNode *tail;
} SymbolTable;

bool isEmptySymbolTable(SymbolTable *st) {
    return st->head == NULL && st->tail == NULL;
}

SymbolTable *newEmptySymbolTable() {
    SymbolTable *st = malloc(sizeof(SymbolTable));
    st->head = NULL;
    st->tail = NULL;
    return st;
}

static Symbol *SymbolTable_get(SymbolTable *st, char *name) {
    for (SymbolNode *q = st->head; q != NULL; q = q->next) {
        if (strcmp(q->symbol->name, name) == 0) {
            return q->symbol;
        }
    }
    return NULL;
}

static bool SymbolTable_contains(SymbolTable *st, char *name) {
    return SymbolTable_get(st, name) != NULL;
}

void SymbolTable_append(SymbolTable *st, Symbol *sym) {
    SymbolNode *node = malloc(sizeof(SymbolNode));
    node->next = NULL;
    node->symbol = sym;
    if (isEmptySymbolTable(st)) {
        st->head = node;
        st->tail = node;
    } else {
        st->tail->next = node;
        st->tail = node;
    }
}

typedef struct Env_ {
    struct Env_ *next;
    SymbolTable *vst; // variable symbol table
    SymbolTable *fst; // function symbol table
} Env;

static Env *cenv;

bool in_nested_env() {
    return cenv->next != NULL;
}

void init_env() {
    cenv = malloc(sizeof(Env));
    cenv->next = NULL;
    cenv->vst = newEmptySymbolTable();
    cenv->fst = newEmptySymbolTable();
    install_function_defined("read", getBasicInt(), NULL);
}

void enter_new_env() {
    Env *env = malloc(sizeof(Env));
    env->next = cenv;
    env->vst = newEmptySymbolTable();
    env->fst = newEmptySymbolTable();
    cenv = env;
}

FieldNode *generateFieldList(Env *cenv) {
    FieldNode *head = NULL;
    FieldNode *tail = NULL;
    SymbolTable *st = cenv->vst;
    for (SymbolNode *q = st->head; q != NULL; q = q->next) {
        Symbol *sym = q->symbol;
        FieldNode *field = newFieldNode(sym->name, sym->type);
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

int contains_variable(char *name) {
    SymbolTable *st = cenv->vst;
    return SymbolTable_contains(st, name);
}

int contains_function_defined(char *name) {
    SymbolTable *st = cenv->fst;
    Symbol *symbol = SymbolTable_get(st, name);
    return symbol != NULL && symbol->state == DEFINED;
}

int contains_function_declared(char *name) {
    SymbolTable *st = cenv->fst;
    Symbol *symbol = SymbolTable_get(st, name);
    return symbol != NULL && symbol->state == DECLARED;
}

void install_variable(char *name, Type *type) {
    if (contains_variable(name)) {
        return;
    }
    Symbol *symbol = newVariableSymbol(name, type);
    SymbolTable *st = cenv->vst;
    SymbolTable_append(st, symbol);
    info("install variable '%s'", name);
}

void install_function_defined(char *name, Type *returnType, TypeNode *paramTypeList) {
    SymbolTable *st = cenv->fst;
    Symbol *symbol = SymbolTable_get(st, name);
    if (symbol == NULL) {
        Symbol *symbol = newFunctionSymbol(name, 0, returnType, paramTypeList);
        symbol->state = DEFINED;
        SymbolTable_append(st, symbol);
        info("install function (defined) '%s'", name);
    } else {
        if (symbol->state == DECLARED) {
            symbol->state = DEFINED;
            info("upgrade function '%s' to defined", name);
        }
    }
}

void install_function_declared(char *name, Type *returnType, 
        TypeNode *paramTypeList, int lineno) {
    SymbolTable *st = cenv->fst;
    Symbol *symbol = SymbolTable_get(st, name);
    if (symbol != NULL) {
        return;
    }
    Symbol *newSym = newFunctionSymbol(name, lineno, 
            returnType, paramTypeList);
    newSym->state = DECLARED;
    SymbolTable_append(st, newSym);
    info("install function (declared) '%s'", name);
}

Type *retrieve_variable_type(char *name) {
    SymbolTable *st = cenv->vst;
    Symbol *symbol = SymbolTable_get(st, name);
    if (symbol == NULL) {
        warn("cannot retrieve variable '%s'", name);
        return getArbitType();
    } else {
        return symbol->type;
    }
}

Type *retrieve_function_returnType(char *name) {
    debug("calling function '%s'", __func__);
    SymbolTable *st = cenv->fst;
    Symbol *symbol = SymbolTable_get(st, name);
    if (symbol == NULL) {
        warn("cannot retrieve function '%s'", name);
        return getArbitType();
    } else {
        return symbol->returnType;
    }
}

TypeNode *retrieve_function_paramTypeList(char *name) {
    debug("calling function '%s'", __func__);
    SymbolTable *st = cenv->fst;
    Symbol *symbol = SymbolTable_get(st, name);
    if (symbol == NULL) {
        fatal("cannot retrieve function '%s'", name);
    } else {
        return symbol->paramTypeList;
    }
}

bool check_function_declared_undefined() {
    bool ret = true;
    SymbolTable *st = cenv->fst;
    for (SymbolNode *q = st->head; q != NULL; q = q->next) {
        Symbol *sym = q->symbol;
        if (sym->state == DECLARED) {
            printf("Error type 18 at Line %d: Function '%s' declared but not defined\n", sym->lineno, sym->name);
            extern int nr_semantics_error;
            nr_semantics_error++;
        }
        ret = false;
    }
    return ret;
}


void print_symbol_table() {
    int i = 0;
    for (SymbolNode *q = cenv->vst->head; q != NULL; q = q->next) {
        i++;
        Symbol *sym = q->symbol;
        printf("[%d] (variable) %s : %s\n", i, sym->name, typeRepr(sym->type));
    }

    int j = 0;
    for (SymbolNode *q = cenv->fst->head; q != NULL; q = q->next) {
        j++;
        Symbol *sym = q->symbol;
        printf("[%d] (function) %s : %s -> %s\n", j, sym->name, 
                typeListRepr(sym->paramTypeList), typeRepr(sym->returnType));
    }
}
