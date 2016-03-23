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
    compSt->type = COMP_ST;
    compSt->defList = defList;
    compSt->stmtList = stmtList;

    return compSt;
}

StmtList *newStmtList() {
    StmtList *stmtList = malloc(sizeof(StmtList));
    stmtList->type = STMT_LIST;
    stmtList->list_stmt = NULL;

    return stmtList;
}

StmtList *StmtList_insert(void *arg0, void *arg1) {
    Stmt *stmt = (Stmt *) arg0;
    StmtList *stmtList = (StmtList *) arg1;

    ListNode *node = newListNode(stmt);
    node->next = stmtList->list_stmt;
    stmtList->list_stmt = node;

    return stmtList;
}

Stmt *newStmt_RETURN(void *arg0) {
    Exp *exp = (Exp *) arg0;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->type = STMT;
    stmt->stmt_type = STMT_T_RETURN;
    stmt->exp = exp;

    return stmt;
}

DefList *newDefList() {
    DefList *defList = malloc(sizeof(DefList));
    defList->type = DEF_LIST;
    defList->list_def = NULL;

    return defList;
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

Exp *newExp_infix(int op, void *arg0, void *arg1) {
    Exp *exp_left = (Exp *) arg0;
    Exp *exp_right = (Exp *) arg1;

    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_INFIX;
    exp->op = op;
    exp->exp_left = exp_left;
    exp->exp_right = exp_right;

    return exp;
}

Exp *newExp_ID(int id_index) {
    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_ID;
    exp->id_index = id_index;

    return exp;
}

Exp *newExp_INT(int int_index) {
    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_INT;
    exp->int_index = int_index;

    return exp;
}

Exp *newExp_FLOAT(int float_index) {
    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_FLOAT;
    exp->float_index = float_index;

    return exp;
}
