#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

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
    program->parent = NULL;
    program->extDefList = extDefList;
    return program;
}

ExtDefList *newExtDefList() {
    ExtDefList *extDefList = malloc(sizeof(ExtDefList));
    extDefList->type = EXT_DEF_LIST;
    extDefList->parent = NULL;
    extDefList->list = NULL;
    return extDefList;
}

ExtDefList *ExtDefList_add(void *arg0, void *arg1) {
    ExtDef *extDef = (ExtDef *)arg0;
    ExtDefList *extDefList = (ExtDefList *)arg1;

    /* insert extDef before head of extDefList->list */
    // TODO
}

VarDec *newVarDec_1(int id_value) {
    VarDec *varDec = malloc(sizeof(VarDec));
    varDec->type = VAR_DEC;
    varDec->parent = NULL;
    varDec->id_index = id_value;
    return varDec;
}

DecList *newDecList(void *arg0) {
    Dec *dec = (Dec *) arg0;
    
    DecList *decList = malloc(sizeof(DecList));
    decList->type = DEC_LIST;
    decList->parent = NULL;
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
    dec->parent = NULL;
    dec->varDec = varDec;

    return dec;
}
