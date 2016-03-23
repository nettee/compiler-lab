#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

typedef void (*funcptr)(void *);

void visit(void *node);

static int indent = -1;

static void print(const char *str) {
    int i;
    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("%s\n", str);
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
}

void visitDef(void *node) {
    print("Def");
}

void visitDecList(void *node) {
    print("DecList");
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
    visit(root);
}
