#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

typedef void (*funcptr)(void *);

void visit(void *node);

void visitProgram(void *node) {
    printf("visitProgram\n");
}

void visitExtDefList(void *node) {
    printf("visitExtDefList\n");
}

void visitExtDef(void *node) {
    printf("visitExtDef\n");
}

void visitExtDecList(void *node) {
    printf("visitExtDecList\n");
}

void visitSpecifier(void *node) {
    printf("visitSpecifier\n");
}

void visitStructSpecifier(void *node) {
    printf("visitStructSpecifier\n");
}

void visitOptTag(void *node) {
    printf("visitOptTag\n");
}

void visitTag(void *node) {
    printf("visitTag\n");
}

void visitVarDec(void *node) {
    printf("visitVarDec\n");
}

void visitFunDec(void *node) {
    printf("visitFunDec\n");
}

void visitVarList(void *node) {
    printf("visitVarList\n");
}

void visitParamDec(void *node) {
    printf("visitParamDec\n");
}

void visitCompSt(void *node) {
    printf("visitCompSt\n");
}

void visitStmtList(void *node) {
    printf("visitStmtList\n");
}

void visitStmt(void *node) {
    printf("visitStmt\n");
}

void visitDefList(void *node) {
    printf("visitDefList\n");
}

void visitDef(void *node) {
    printf("visitDef\n");
}

void visitDecList(void *node) {
    printf("visitDecList\n");
}

void visitDec(void *node) {
    printf("visitDec\n");
    Dec *dec = (Dec *)node;
    visit(dec->varDec);
}

void visitExp(void *node) {
    printf("visitExp\n");
}

void visitArgs(void *node) {
    printf("visitArgs\n");
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
    printf("type = %d\n", type);
    visitor_table[type-400](node);
}

void print_ast() {
    visit(root);
}

