#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "ast.h"

typedef void (*funcptr)(void *);

void visit(void *node);

char *resolve_id(int);
void print_symbol_table();

static int indent = -1;

#define print(...) { \
    int i; \
    for (i = 0; i < indent; i++) { \
        printf("  "); \
    } \
    printf(__VA_ARGS__); \
    printf("\n"); \
}

void visitProgram(void *node) {
    print("Program");
}

void visitExtDefList(void *node) {
    print("ExtDefList");
}

void visitExtDef(void *node) {
    print("ExtDef");
}

void visitExtDecList(void *node) {
    print("ExtDecList");
}

void visitSpecifier(void *node) {
    print("Specifier");
    Specifier *specifier = (Specifier *)node;
    switch (specifier->type_value) {
    case T_INT:
        print("  TYPE: int");
        break;
    case T_FLOAT:
        print("  TYPE: float");
        break;
    default:
        printf("Fatal: Unknown specifier->type_value\n");
    }
}

void visitStructSpecifier(void *node) {
    print("StructSpecifier");
}

void visitOptTag(void *node) {
    print("OptTag");
}

void visitTag(void *node) {
    print("Tag");
}

void visitVarDec(void *node) {
    print("VarDec");
    VarDec *varDec = (VarDec *)node;
    print("  ID: %s", resolve_id(varDec->id_index));
}

void visitFunDec(void *node) {
    print("FunDec");
}

void visitVarList(void *node) {
    print("VarList");
}

void visitParamDec(void *node) {
    print("ParamDec");
}

void visitCompSt(void *node) {
    print("CompSt");
}

void visitStmtList(void *node) {
    print("StmtList");
}

void visitStmt(void *node) {
    print("Stmt");
}

void visitDefList(void *node) {
    print("DefList");
    DefList *defList = (DefList *)node;
    for (ListNode *q = defList->list_def; q != NULL; q = q->next) {
        visit(q->child);
    }
}

void visitDef(void *node) {
    print("Def");
    Def *def = (Def *)node;
    visit(def->specifier);
    visit(def->decList);
}

void visitDecList(void *node) {
    print("DecList");
    DecList *decList = (DecList *)node;
    for (ListNode *q = decList->list_dec; q != NULL; q = q->next) {
        visit(q->child);
    }
}

void visitDec(void *node) {
    print("Dec");
    Dec *dec = (Dec *)node;
    visit(dec->varDec);
}

void visitExp(void *node) {
    print("Exp");
}

void visitArgs(void *node) {
    print("Args");
}

funcptr visitor_table[] = {
    visitProgram,
    visitExtDefList,
    visitExtDef,
    visitExtDecList,
    visitSpecifier,
    visitStructSpecifier,
    visitOptTag,
    visitTag,
    visitVarDec,
    visitFunDec,
    visitVarList,
    visitParamDec,
    visitCompSt,
    visitStmtList,
    visitStmt,
    visitDefList,
    visitDef,
    visitDecList,
    visitDec,
    visitExp,
    visitArgs,
};

void visit(void *node) {
    //printf("node = %p\n", node);
    int type = *(int *)node;
    //printf("type = %d\n", type);
    indent++;
    visitor_table[type-400](node);
    indent--;
}

void print_ast() {
    print_symbol_table();
    visit(root);
}

