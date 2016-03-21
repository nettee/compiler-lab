#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

void *root;

typedef void (*funcptr)();

void visitProgram() {
    printf("visitProgram\n");
}

void visitExtDefList() {
    printf("visitExtDefList\n");
}

void visitExtDef() {
    printf("visitExtDef\n");
}

void visitExtDecList() {
    printf("visitExtDecList\n");
}

funcptr visitor_table[] = {
    visitProgram, 
    visitExtDefList, 
    visitExtDef,
    visitExtDecList,
};

Program *newProgram(void *extDefList) {
    Program *program = malloc(sizeof(Program));
    program->type = PROGRAM;
    program->parent = NULL;
    program->extDefList = extDefList;
    return program;
}

static void visit(void *node) {
    int type = *(int *)node;
    printf("type = %d\n", type);
    visitor_table[type-400]();
}

void print_ast() {
    visit(root);
}

