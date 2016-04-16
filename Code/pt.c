#include <stdio.h>
#include <stdlib.h>

#include "pt.h"
#include "common.h"

void *root;

Program *newProgram(void *arg0, int lineno) {
    ExtDefList *extDefList = (ExtDefList *) arg0;

    Program *program = malloc(sizeof(Program));
    program->kind = PROGRAM;
    program->lineno = lineno;
    program->extDefList = extDefList;
    
    return program;
}

ExtDefList *newExtDefList(void *arg0, void *arg1, int lineno) {
    ExtDef *extDef = (ExtDef *) arg0;
    ExtDefList *extDefList0 = (ExtDefList *) arg1;

    ExtDefList *extDefList = malloc(sizeof(ExtDefList));
    extDefList->kind = EXT_DEF_LIST;
    extDefList->lineno = lineno;
    extDefList->extDef = extDef;
    extDefList->extDefList = extDefList0;

    return extDefList;
}

ExtDef *newExtDef_var(void *arg0, void *arg1, int lineno) {
    Specifier *specifier = (Specifier *) arg0;
    ExtDecList *extDecList = (ExtDecList *) arg1;

    ExtDef *extDef = malloc(sizeof(ExtDef));
    extDef->kind = EXT_DEF;
    extDef->lineno = lineno;
    extDef->extdef_kind = EXT_DEF_T_VAR;
    extDef->var.specifier = specifier;
    extDef->var.extDecList = extDecList;

    return extDef;
}


ExtDef *newExtDef_struct(void *arg0, int lineno) {
    Specifier *specifier = (Specifier *) arg0;

    ExtDef *extDef = malloc(sizeof(ExtDef));
    extDef->kind = EXT_DEF;
    extDef->lineno = lineno;
    extDef->extdef_kind = EXT_DEF_T_STRUCT;
    extDef->struct_.specifier = specifier;

    return extDef;
}

ExtDef *newExtDef_fun(void *arg0, void *arg1, void *arg2, int lineno) {
    Specifier *specifier = (Specifier *) arg0;
    FunDec *funDec = (FunDec *) arg1;
    CompSt *compSt = (CompSt *) arg2;

    ExtDef *extDef = malloc(sizeof(ExtDef));
    extDef->kind = EXT_DEF;
    extDef->lineno = lineno;
    extDef->extdef_kind = EXT_DEF_T_FUN;
    extDef->fun.specifier = specifier;
    extDef->fun.funDec = funDec;
    extDef->fun.compSt = compSt;

    return extDef;
}

ExtDecList *newExtDecList(void *arg0, void *arg1, int lineno) {
    VarDec *varDec = (VarDec *) arg0;
    ExtDecList *extDecList0 = (ExtDecList *) arg1;

    ExtDecList *extDecList = malloc(sizeof(ExtDecList));
    extDecList->kind = EXT_DEC_LIST;
    extDecList->lineno = lineno;
    extDecList->varDec = varDec;
    extDecList->extDecList = extDecList0;

    return extDecList;
}

Specifier *newSpecifier_basic(int type_index, int lineno) {
    Specifier *specifier = malloc(sizeof(Specifier));
    specifier->kind = SPECIFIER;
    specifier->lineno = lineno;
    specifier->specifier_kind = SPECIFIER_T_BASIC;
    specifier->type_index = type_index;

    return specifier;
}

Specifier *newSpecifier_struct(void *arg0, int lineno) {
    StructSpecifier *structSpecifier = (StructSpecifier *) arg0;

    Specifier *specifier = malloc(sizeof(Specifier));
    specifier->kind = SPECIFIER;
    specifier->lineno = lineno;
    specifier->specifier_kind = SPECIFIER_T_STRUCT;
    specifier->structSpecifier = structSpecifier;

    return specifier;
}

StructSpecifier *newStructSpecifier_dec(void *arg0, int lineno) {
    Tag *tag = (Tag *) arg0;

    StructSpecifier *structSpecifier = malloc(sizeof(StructSpecifier));
    structSpecifier->kind = STRUCT_SPECIFIER;
    structSpecifier->lineno = lineno;
    structSpecifier->structspecifier_kind = STRUCT_SPECIFIER_T_DEC;
    structSpecifier->dec.tag = tag;

    return structSpecifier;
}

StructSpecifier *newStructSpecifier_def(void *arg0, void *arg1, int lineno) {
    OptTag *optTag = (OptTag *) arg0;
    DefList *defList = (DefList *) arg1;

    StructSpecifier *structSpecifier = malloc(sizeof(StructSpecifier));
    structSpecifier->kind = STRUCT_SPECIFIER;
    structSpecifier->lineno = lineno;
    structSpecifier->structspecifier_kind = STRUCT_SPECIFIER_T_DEF;
    structSpecifier->def.optTag = optTag;
    structSpecifier->def.defList = defList;

    return structSpecifier;
}

OptTag *newOptTag(char *id_text, int lineno) {
    OptTag *optTag = malloc(sizeof(OptTag));
    optTag->kind = OPT_TAG;
    optTag->lineno = lineno;
    optTag->id_text = id_text;

    return optTag;
}

Tag *newTag(char *id_text, int lineno) {
    Tag *tag = malloc(sizeof(Tag));
    tag->kind = TAG;
    tag->lineno = lineno;
    tag->id_text = id_text;

    return tag;
}

VarDec *newVarDec_ID(char *id_text, int lineno) {
    VarDec *varDec = malloc(sizeof(VarDec));
    varDec->kind = VAR_DEC;
    varDec->lineno = lineno;
    varDec->vardec_kind = VAR_DEC_T_ID;
    varDec->id_text = id_text;

    return varDec;
}

VarDec *newVarDec_dim(void *arg0, int int_value, int lineno) {
    VarDec *varDec0 = (VarDec *) arg0;

    VarDec *varDec = malloc(sizeof(VarDec));
    varDec->kind = VAR_DEC;
    varDec->lineno = lineno;
    varDec->vardec_kind = VAR_DEC_T_DIM;
    varDec->dim.varDec = varDec0;
    varDec->dim.int_value = int_value;

    return varDec;
}

FunDec *newFunDec(char *id_text, void *arg0, int lineno) {
    VarList *varList = (VarList *) arg0;

    FunDec *funDec = malloc(sizeof(FunDec));
    funDec->kind = FUN_DEC;
    funDec->lineno = lineno;
    funDec->id_text = id_text;
    funDec->varList = varList;

    return funDec;
}

VarList *newVarList(void *arg0, void *arg1, int lineno) {
    ParamDec *paramDec = (ParamDec *) arg0;
    VarList *varList0 = (VarList *) arg1;

    VarList *varList = malloc(sizeof(VarList));
    varList->kind = VAR_LIST;
    varList->lineno = lineno;
    varList->paramDec = paramDec;
    varList->varList = varList0;

    return varList;
}

ParamDec *newParamDec(void *arg0, void *arg1, int lineno) {
    Specifier *specifier = (Specifier *) arg0;
    VarDec *varDec = (VarDec *) arg1;

    ParamDec *paramDec = malloc(sizeof(ParamDec));
    paramDec->kind = PARAM_DEC;
    paramDec->lineno = lineno;
    paramDec->specifier = specifier;
    paramDec->varDec = varDec;

    return paramDec;
}

CompSt *newCompSt(void *arg0, void *arg1, int lineno) {
    DefList *defList = (DefList *) arg0;
    StmtList *stmtList = (StmtList *) arg1;

    CompSt *compSt = malloc(sizeof(CompSt));
    compSt->kind = COMP_ST;
    compSt->lineno = lineno;
    compSt->defList = defList;
    compSt->stmtList = stmtList;

    return compSt;
}

StmtList *newStmtList(void *arg0, void *arg1, int lineno) {
    Stmt *stmt = (Stmt *) arg0;
    StmtList *stmtList0 = (StmtList *) arg1;

    StmtList *stmtList = malloc(sizeof(StmtList));
    stmtList->kind = STMT_LIST;
    stmtList->lineno = lineno;
    stmtList->stmt = stmt;
    stmtList->stmtList = stmtList0;

    return stmtList;
}

Stmt *newStmt_exp(void *arg0, int lineno) {
    Exp *exp = (Exp *) arg0;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->kind = STMT;
    stmt->lineno = lineno;
    stmt->stmt_kind = STMT_T_EXP;
    stmt->exp.exp = exp;

    return stmt;
}

Stmt *newStmt_COMP_ST(void *arg0, int lineno) {
    CompSt *compSt = (CompSt *) arg0;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->kind = STMT;
    stmt->lineno = lineno;
    stmt->stmt_kind = STMT_T_COMP_ST;
    stmt->compst.compSt = compSt;

    return stmt;
}

Stmt *newStmt_RETURN(void *arg0, int lineno) {
    Exp *exp = (Exp *) arg0;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->kind = STMT;
    stmt->lineno = lineno;
    stmt->stmt_kind = STMT_T_RETURN;
    stmt->return_.exp = exp;

    return stmt;
}

Stmt *newStmt_if(void *arg0, void *arg1, int lineno) {
    Exp *exp = (Exp *) arg0;
    Stmt *then_stmt = (Stmt *) arg1;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->kind = STMT;
    stmt->lineno = lineno;
    stmt->stmt_kind = STMT_T_IF;
    stmt->if_.exp = exp;
    stmt->if_.then_stmt = then_stmt;

    return stmt;
}

Stmt *newStmt_ifelse(void *arg0, void *arg1, void *arg2, int lineno) {
    Exp *exp = (Exp *) arg0;
    Stmt *then_stmt = (Stmt *) arg1;
    Stmt *else_stmt = (Stmt *) arg2;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->kind = STMT;
    stmt->lineno = lineno;
    stmt->stmt_kind = STMT_T_IF_ELSE;
    stmt->ifelse.exp = exp;
    stmt->ifelse.then_stmt = then_stmt;
    stmt->ifelse.else_stmt = else_stmt;

    return stmt;
}

Stmt *newStmt_WHILE(void *arg0, void *arg1, int lineno) {
    Exp *exp = (Exp *) arg0;
    Stmt *body_stmt = (Stmt *) arg1;

    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->kind = STMT;
    stmt->lineno = lineno;
    stmt->stmt_kind = STMT_T_WHILE;
    stmt->while_.exp = exp;
    stmt->while_.stmt = body_stmt;

    return stmt;
}

DefList *newDefList(void *arg0, void *arg1, int lineno) {
    Def *def = (Def *) arg0;
    DefList *defList0 = (DefList *) arg1;

    DefList *defList = malloc(sizeof(DefList));
    defList->kind = DEF_LIST;
    defList->lineno = lineno;
    defList->def = def;
    defList->defList = defList0;

    return defList;
}

Def *newDef(void *arg0, void *arg1, int lineno) {
    Specifier *specifier = (Specifier *) arg0;
    DecList *decList = (DecList *) arg1;

    Def *def = malloc(sizeof(Def));
    def->kind = DEF;
    def->lineno = lineno;
    def->specifier = specifier;
    def->decList = decList;

    return def;
}

DecList *newDecList(void *arg0, void *arg1, int lineno) {
    Dec *dec = (Dec *) arg0;
    DecList *decList0 = (DecList *) arg1;

    DecList *decList = malloc(sizeof(DecList));
    decList->kind = DEC_LIST;
    decList->lineno = lineno;
    decList->dec = dec;
    decList->decList = decList0;

    return decList;
}

Dec *newDec(void *arg0, void *arg1, int lineno) {
    VarDec *varDec = (VarDec *) arg0;
    Exp *exp = (Exp *) arg1;
    
    Dec *dec = malloc(sizeof(Dec));
    dec->kind = DEC;
    dec->lineno = lineno;
    dec->varDec = varDec;
    dec->exp = exp;

    return dec;
}

Exp *newExp_infix(int op, void *arg0, int op_yylval, void *arg1, int lineno) {
    Exp *exp_left = (Exp *) arg0;
    Exp *exp_right = (Exp *) arg1;

    Exp *exp = malloc(sizeof(Exp));
    exp->kind = EXP;
    exp->lineno = lineno;
    exp->exp_kind = EXP_T_INFIX;
    exp->infix.op = op;
    exp->infix.op_yylval = op_yylval;
    exp->infix.exp_left = exp_left;
    exp->infix.exp_right = exp_right;

    return exp;
}

Exp *newExp_paren(void *arg0, int lineno) {
    Exp *parened_exp = (Exp *) arg0;

    Exp *exp = malloc(sizeof(Exp));
    exp->kind = EXP;
    exp->lineno = lineno;
    exp->exp_kind = EXP_T_PAREN;
    exp->paren.exp = parened_exp;

    return exp;
}

Exp *newExp_unary(int op, void *arg0, int lineno) {
    Exp *unary_exp = (Exp *) arg0;

    Exp *exp = malloc(sizeof(Exp));
    exp->kind = EXP;
    exp->lineno = lineno;
    exp->exp_kind = EXP_T_UNARY;
    exp->unary.op = op;
    exp->unary.exp = unary_exp;
    
    return exp;
}

Exp *newExp_call(char *id_text, void *arg0, int lineno) {
    Args *args = (Args *) arg0;

    Exp *exp = malloc(sizeof(Exp));
    exp->kind = EXP;
    exp->lineno = lineno;
    exp->exp_kind = EXP_T_CALL;
    exp->call.id_text = id_text;
    exp->call.args = args;

    return exp;
}

Exp *newExp_subscript(void *arg0, void *arg1, int lineno) {
    Exp *array = (Exp *) arg0;
    Exp *index = (Exp *) arg1;

    Exp *exp = malloc(sizeof(Exp));
    exp->kind = EXP;
    exp->lineno = lineno;
    exp->exp_kind = EXP_T_SUBSCRIPT;
    exp->subscript.array = array;
    exp->subscript.index = index;

    return exp;
}

Exp *newExp_DOT(void *arg0, char *id_text, int lineno) {
    Exp *dotted_exp = (Exp *) arg0;

    Exp *exp = malloc(sizeof(Exp));
    exp->kind = EXP;
    exp->lineno = lineno;
    exp->exp_kind = EXP_T_DOT;
    exp->dot.exp = dotted_exp;
    exp->dot.id_text = id_text;

    return exp;
}

Exp *newExp_ID(char *id_text, int lineno) {
    Exp *exp = malloc(sizeof(Exp));
    exp->kind = EXP;
    exp->lineno = lineno;
    exp->exp_kind = EXP_T_ID;
    exp->id_text = id_text;

    return exp;
}

Exp *newExp_INT(int int_value, int lineno) {
    Exp *exp = malloc(sizeof(Exp));
    exp->kind = EXP;
    exp->lineno = lineno;
    exp->exp_kind = EXP_T_INT;
    exp->int_value = int_value;

    return exp;
}

Exp *newExp_FLOAT(float float_value, int lineno) {
    Exp *exp = malloc(sizeof(Exp));
    exp->kind = EXP;
    exp->lineno = lineno;
    exp->exp_kind = EXP_T_FLOAT;
    exp->float_value = float_value;

    return exp;
}

Args *newArgs(void *arg0, void *arg1, int lineno) {
    Exp *exp = (Exp *) arg0;
    Args *args0 = (Args *) arg1;

    Args *args = malloc(sizeof(Args));
    args->kind = ARGS;
    args->lineno = lineno;
    args->exp = exp;
    args->args = args0;

    return args;
}

