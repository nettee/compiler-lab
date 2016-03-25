#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "common.h"

void *root;

Program *newProgram(void *arg0) {
    ExtDefList *extDefList = (ExtDefList *) arg0;

    Program *program = malloc(sizeof(Program));
    program->type = PROGRAM;
    program->extDefList = extDefList;
    
    return program;
}

ExtDefList *newExtDefList(void *arg0, void *arg1) {
    ExtDef *extDef = (ExtDef *) arg0;
    ExtDefList *extDefList0 = (ExtDefList *) arg1;

    ExtDefList *extDefList = malloc(sizeof(ExtDefList));
    extDefList->type = EXT_DEF_LIST;
    extDefList->extDef = extDef;
    extDefList->extDefList = extDefList0;

    return extDefList;
}

ExtDef *newExtDef_var(void *arg0, void *arg1) {
    Specifier *specifier = (Specifier *) arg0;
    ExtDecList *extDecList = (ExtDecList *) arg1;

    ExtDef *extDef = malloc(sizeof(ExtDef));
    extDef->type = EXT_DEF;
    extDef->extdef_type = EXT_DEF_T_VAR;
    extDef->var.specifier = specifier;
    extDef->var.extDecList = extDecList;

    return extDef;
}


ExtDef *newExtDef_struct(void *arg0) {
    Specifier *specifier = (Specifier *) arg0;

    ExtDef *extDef = malloc(sizeof(ExtDef));
    extDef->type = EXT_DEF;
    extDef->extdef_type = EXT_DEF_T_STRUCT;
    extDef->struct_.specifier = specifier;

    return extDef;
}

ExtDef *newExtDef_fun(void *arg0, void *arg1, void *arg2) {
    Specifier *specifier = (Specifier *) arg0;
    FunDec *funDec = (FunDec *) arg1;
    CompSt *compSt = (CompSt *) arg2;

    ExtDef *extDef = malloc(sizeof(ExtDef));
    extDef->type = EXT_DEF;
    extDef->extdef_type = EXT_DEF_T_FUN;
    extDef->fun.specifier = specifier;
    extDef->fun.funDec = funDec;
    extDef->fun.compSt = compSt;

    return extDef;
}

ExtDecList *newExtDecList(void *arg0, void *arg1) {
    VarDec *varDec = (VarDec *) arg0;
    ExtDecList *extDecList0 = (ExtDecList *) arg1;

    ExtDecList *extDecList = malloc(sizeof(ExtDecList));
    extDecList->type = EXT_DEC_LIST;
    extDecList->varDec = varDec;
    extDecList->extDecList = extDecList0;

    return extDecList;
}

Specifier *newSpecifier_TYPE(int type_index) {
    Specifier *specifier = malloc(sizeof(Specifier));
    specifier->type = SPECIFIER;
    specifier->specifier_type = SPECIFIER_T_TYPE;
    specifier->type_index = type_index;

    return specifier;
}

Specifier *newSpecifier_struct(void *arg0) {
    StructSpecifier *structSpecifier = (StructSpecifier *) arg0;

    Specifier *specifier = malloc(sizeof(Specifier));
    specifier->type = SPECIFIER;
    specifier->specifier_type = SPECIFIER_T_STRUCT;
    specifier->structSpecifier = structSpecifier;

    return specifier;
}

StructSpecifier *newStructSpecifier_dec(void *arg0) {
    Tag *tag = (Tag *) arg0;

    StructSpecifier *structSpecifier = malloc(sizeof(StructSpecifier));
    structSpecifier->type = STRUCT_SPECIFIER;
    structSpecifier->structspecifier_type = STRUCT_SPECIFIER_T_DEC;
    structSpecifier->dec.tag = tag;

    return structSpecifier;
}

StructSpecifier *newStructSpecifier_def(void *arg0, void *arg1) {
    OptTag *optTag = (OptTag *) arg0;
    DefList *defList = (DefList *) arg1;

    StructSpecifier *structSpecifier = malloc(sizeof(StructSpecifier));
    structSpecifier->type = STRUCT_SPECIFIER;
    structSpecifier->structspecifier_type = STRUCT_SPECIFIER_T_DEF;
    structSpecifier->def.optTag = optTag;
    structSpecifier->def.defList = defList;

    return structSpecifier;
}

OptTag *newOptTag(int id_index) {
    OptTag *optTag = malloc(sizeof(OptTag));
    optTag->type = OPT_TAG;
    optTag->has_id = 1; // true
    optTag->id_index = id_index;

    return optTag;
}

OptTag *newOptTag_empty() {
    OptTag *optTag = malloc(sizeof(OptTag));
    optTag->type = OPT_TAG;
    optTag->has_id = 0; // false
    optTag->id_index = -1;

    return optTag;
}

Tag *newTag(int id_index) {
    Tag *tag = malloc(sizeof(Tag));
    tag->type = TAG;
    tag->id_index = id_index;

    return tag;
}

VarDec *newVarDec_ID(int id_index) {
    VarDec *varDec = malloc(sizeof(VarDec));
    varDec->type = VAR_DEC;
    varDec->vardec_type = VAR_DEC_T_ID;
    varDec->id_index = id_index;

    return varDec;
}

VarDec *newVarDec_dim(void *arg0, int int_index) {
    VarDec *varDec0 = (VarDec *) arg0;

    VarDec *varDec = malloc(sizeof(VarDec));
    varDec->type = VAR_DEC;
    varDec->vardec_type = VAR_DEC_T_DIM;
    varDec->dim.varDec = varDec0;
    varDec->dim.int_index = int_index;

    return varDec;
}

FunDec *newFunDec(int id_index, void *arg0) {
    VarList *varList = (VarList *) arg0;

    FunDec *funDec = malloc(sizeof(FunDec));
    funDec->type = FUN_DEC;
    funDec->id_index = id_index;
    funDec->varList = varList;

    return funDec;
}

VarList *newVarList(void *arg0, void *arg1) {
    ParamDec *paramDec = (ParamDec *) arg0;
    VarList *varList0 = (VarList *) arg1;

    VarList *varList = malloc(sizeof(VarList));
    varList->type = VAR_LIST;
    varList->paramDec = paramDec;
    varList->varList = varList0;

    return varList;
}

ParamDec *newParamDec(void *arg0, void *arg1) {
    Specifier *specifier = (Specifier *) arg0;
    VarDec *varDec = (VarDec *) arg1;

    ParamDec *paramDec = malloc(sizeof(ParamDec));
    paramDec->type = PARAM_DEC;
    paramDec->specifier = specifier;
    paramDec->varDec = varDec;

    return paramDec;
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

StmtList *newStmtList(void *arg0, void *arg1) {
    Stmt *stmt = (Stmt *) arg0;
    StmtList *stmtList0 = (StmtList *) arg1;

    StmtList *stmtList = malloc(sizeof(StmtList));
    stmtList->type = STMT_LIST;
    stmtList->stmt = stmt;
    stmtList->stmtList = stmtList0;

    return stmtList;
}

Stmt *newStmt_exp(void *arg0) {
    Exp *exp = (Exp *) arg0;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->type = STMT;
    stmt->stmt_type = STMT_T_EXP;
    stmt->exp.exp = exp;

    return stmt;
}

Stmt *newStmt_COMP_ST(void *arg0) {
    CompSt *compSt = (CompSt *) arg0;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->type = STMT;
    stmt->stmt_type = STMT_T_COMP_ST;
    stmt->compst.compSt = compSt;

    return stmt;
}

Stmt *newStmt_RETURN(void *arg0) {
    Exp *exp = (Exp *) arg0;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->type = STMT;
    stmt->stmt_type = STMT_T_RETURN;
    stmt->return_.exp = exp;

    return stmt;
}

Stmt *newStmt_if(void *arg0, void *arg1) {
    Exp *exp = (Exp *) arg0;
    Stmt *then_stmt = (Stmt *) arg1;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->type = STMT;
    stmt->stmt_type = STMT_T_IF;
    stmt->if_.exp = exp;
    stmt->if_.then_stmt = then_stmt;

    return stmt;
}

Stmt *newStmt_ifelse(void *arg0, void *arg1, void *arg2) {
    Exp *exp = (Exp *) arg0;
    Stmt *then_stmt = (Stmt *) arg1;
    Stmt *else_stmt = (Stmt *) arg2;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->type = STMT;
    stmt->stmt_type = STMT_T_IF_ELSE;
    stmt->ifelse.exp = exp;
    stmt->ifelse.then_stmt = then_stmt;
    stmt->ifelse.else_stmt = else_stmt;

    return stmt;
}

Stmt *newStmt_WHILE(void *arg0, void *arg1) {
    Exp *exp = (Exp *) arg0;
    Stmt *body_stmt = (Stmt *) arg1;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->type = STMT;
    stmt->stmt_type = STMT_T_WHILE;
    stmt->while_.exp = exp;
    stmt->while_.stmt = body_stmt;

    return stmt;
}

DefList *newDefList(void *arg0, void *arg1) {
    Def *def = (Def *) arg0;
    DefList *defList0 = (DefList *) arg1;

    DefList *defList = malloc(sizeof(DefList));
    defList->type = DEF_LIST;
    defList->def = def;
    defList->defList = defList0;

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

DecList *newDecList(void *arg0, void *arg1) {
    Dec *dec = (Dec *) arg0;
    DecList *decList0 = (DecList *) arg1;

    DecList *decList = malloc(sizeof(DecList));
    decList->type = DEC_LIST;
    decList->dec = dec;
    decList->decList = decList0;

    return decList;
}

Dec *newDec(void *arg0, void *arg1) {
    VarDec *varDec = (VarDec *) arg0;
    Exp *exp = (Exp *) arg1;
    
    Dec *dec = malloc(sizeof(Dec));
    dec->type = DEC;
    dec->varDec = varDec;
    dec->exp = exp;

    return dec;
}

Exp *newExp_infix(int op, void *arg0, void *arg1) {
    Exp *exp_left = (Exp *) arg0;
    Exp *exp_right = (Exp *) arg1;

    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_INFIX;
    exp->infix.op = op;
    exp->infix.exp_left = exp_left;
    exp->infix.exp_right = exp_right;

    return exp;
}

Exp *newExp_paren(void *arg0) {
    Exp *parened_exp = (Exp *) arg0;

    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_PAREN;
    exp->paren.exp = parened_exp;

    return exp;
}

Exp *newExp_unary(int op, void *arg0) {
    Exp *unary_exp = (Exp *) arg0;

    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_UNARY;
    exp->unary.op = op;
    exp->unary.exp = unary_exp;
    
    return exp;
}

Exp *newExp_call(int id_index, void *arg0) {
    Args *args = (Args *) arg0;

    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_CALL;
    exp->call.id_index = id_index;
    exp->call.args = args;

    return exp;
}

Exp *newExp_subscript(void *arg0, void *arg1) {
    Exp *array = (Exp *) arg0;
    Exp *index = (Exp *) arg1;

    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_SUBSCRIPT;
    exp->subscript.array = array;
    exp->subscript.index = index;

    return exp;
}

Exp *newExp_DOT(void *arg0, int id_index) {
    Exp *dotted_exp = (Exp *) arg0;

    Exp *exp = malloc(sizeof(Exp));
    exp->type = EXP;
    exp->exp_type = EXP_T_DOT;
    exp->dot.exp = dotted_exp;
    exp->dot.id_index = id_index;

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

Args *newArgs(void *arg0, void *arg1) {
    Exp *exp = (Exp *) arg0;
    Args *args0 = (Args *) arg1;

    Args *args = malloc(sizeof(Args));
    args->type = ARGS;
    args->exp = exp;
    args->args = args0;

    return args;
}
