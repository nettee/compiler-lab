#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "common.h"

void *root;

static ListNode *newListNode(void *child) {
    ListNode *listNode = malloc(sizeof(ListNode));
    listNode->next = NULL;
    listNode->child = child;
    return listNode;
}

Program *newProgram(void *extDefList) {
    Program *program = malloc(sizeof(Program));
    program->type = PROGRAM;

    program->extDefList = extDefList;
    return program;
}

ExtDefList *newExtDefList() {
    ExtDefList *extDefList = malloc(sizeof(ExtDefList));
    extDefList->type = EXT_DEF_LIST;

    extDefList->list = NULL;
    return extDefList;
}

ExtDefList *ExtDefList_add(void *arg0, void *arg1) {
    ExtDef *extDef = (ExtDef *)arg0;
    ExtDefList *extDefList = (ExtDefList *)arg1;

    /* insert extDef before head of extDefList->list */
    // TODO
}

Specifier *newSpecifier_1(int type_value) {
    Specifier *specifier = malloc(sizeof(Specifier));

    specifier->type = SPECIFIER;
    specifier->type_value = type_value;

    return specifier;
}

VarDec *newVarDec_1(int id_value) {
    VarDec *varDec = malloc(sizeof(VarDec));
    varDec->type = VAR_DEC;

    varDec->id_index = id_value;
    return varDec;
}

CompSt *newCompSt(void *arg0, void *arg1) {
    DefList *defList = (DefList *) arg0;
    StmtList *stmtList = (StmtList *) arg1;

    CompSt *compSt = malloc(sizeof(CompSt));
    compSt->defList = defList;
    compSt->stmtList = stmtList;

    return compSt;
}

DefList *newDefList() {
    DefList *defList = malloc(sizeof(DefList));
    defList->type = DEF_LIST;
    defList->list_def = NULL;
}

DefList *DefList_insert(void *arg0, void *arg1) {
    Def *def = (Def *) arg0;
    DefList *defList = (DefList *) arg1;

    ListNode *node = newListNode(def);
    node->next = defList->list_def;
    defList->list_def = node;

    return defList;
}

Def *newDef(void *arg0, void *arg1) {
    Specifier *specifier = (Specifier *) arg0;
    DecList *decList = (DecList *) arg1;

    Def *def = malloc(sizeof(Def));
    def->type = DEF;
    def->specifier = specifier;
    def->decList = decList;

    return def;
}

DecList *newDecList(void *arg0) {
    Dec *dec = (Dec *) arg0;
    
    DecList *decList = malloc(sizeof(DecList));
    decList->type = DEC_LIST;
    decList->list_dec = newListNode(dec);

    return decList;
}

DecList *DecList_insert(void *arg0, void *arg1) {
    Dec *dec = (Dec *) arg0;
    DecList *decList = (DecList *) arg1;

    /* insert node in front of list_dec */
    ListNode *node = newListNode(dec);
    node->next = decList->list_dec;
    decList->list_dec = node;

    return decList;
}

Dec *newDec_1(void *arg0) {
    VarDec *varDec = (VarDec *)arg0;
    
    Dec *dec = malloc(sizeof(Dec));
    dec->type = DEC;

    dec->varDec = varDec;

    return dec;
}

Exp *newExp_ID(int id_value) {
    return NULL;
}
