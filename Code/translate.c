#include "common.h"
#include "ir.h"
#include "syntax.tab.h"
#include "pt.h"

extern FILE *ir_out_file;

typedef struct IRNode_ IRNode;

struct IRNode_ {
    IRNode *next;
    IR *ir;
};

typedef struct {
    IRNode *head;
    IRNode *tail;
} IRList;

static void gen(IR *ir) {
    char *repr = ir_repr(ir);
    printf("%s\n", repr);
    fprintf(ir_out_file, "%s\n", repr);
}

static void translate_Exp(Exp *exp, Operand *place);
static void translate_Stmt(Stmt *stmt);
static void translate_Condition(Exp *exp, Label *L1, Label *L2);

static void visit(void *node);

typedef void (*funcptr)(void *);

static void visitProgram(void *node) {
    Program *program = (Program *)node;
    visit(program->extDefList);
}

static void visitExtDefList(void *node) {
    /* may produce epsilon */
    ExtDefList *extDefList = (ExtDefList *)node;
    if (extDefList->extDef != NULL) {
        visit(extDefList->extDef);
        visit(extDefList->extDefList);
    }
}

static void visitExtDef(void *node) {
    ExtDef *extDef = (ExtDef *)node;

    if (extDef->extdef_kind == EXT_DEF_T_VAR) {
        visit(extDef->var.specifier);
        visit(extDef->var.extDecList);

    } else if (extDef->extdef_kind == EXT_DEF_T_STRUCT) {
        visit(extDef->struct_.specifier);

    } else if (extDef->extdef_kind == EXT_DEF_T_FUN) {
        visit(extDef->fun.specifier);
        gen(newFunction(extDef->fun.funDec->id_text));
        visit(extDef->fun.funDec);
        visit(extDef->fun.compSt);

    } else if (extDef->extdef_kind == EXT_DEF_T_FUN_DEC) {
        visit(extDef->fun.specifier);
        visit(extDef->fun.funDec);

    } else {
        fatal("unknown extdef_type");
    }
}

static void visitExtDecList(void *node) {
    ExtDecList *extDecList = (ExtDecList *)node;
    visit(extDecList->varDec);
    if (extDecList->extDecList != NULL) {
        visit(extDecList->extDecList);
    }
}

static void visitSpecifier(void *node) {
    Specifier *specifier = (Specifier *)node;
    if (specifier->specifier_kind == SPECIFIER_T_BASIC) {

    } else if (specifier->specifier_kind ==  SPECIFIER_T_STRUCT) {
        visit(specifier->structSpecifier);

    } else {
        fatal("unknown specifier_type");
    }
}

static void visitStructSpecifier(void *node) {
    StructSpecifier *structSpecifier = (StructSpecifier *)node;
    int kind = structSpecifier->structspecifier_kind;
    if (kind == STRUCT_SPECIFIER_T_DEC) {
        visit(structSpecifier->dec.tag);

    } else if (kind == STRUCT_SPECIFIER_T_DEF) {
        visit(structSpecifier->def.optTag);
        visit(structSpecifier->def.defList);

    } else {
        fatal("unknown structspecifier_type");
    }
}

static void visitOptTag(void *node) {
    /* may produce epsilon */
    OptTag *optTag = (OptTag *)node;
}

static void visitTag(void *node) {
    Tag *tag = (Tag *)node;
}

static void visitVarDec(void *node) {
    VarDec *varDec = (VarDec *)node;
    if (varDec->vardec_kind == VAR_DEC_T_ID) {

    } else if (varDec->vardec_kind == VAR_DEC_T_DIM) {
        visit(varDec->dim.varDec);

    } else {
        fatal("unknown vardec_type");
    }
}

static void visitFunDec(void *node) {
    FunDec *funDec = (FunDec *)node;
    if (funDec->varList != NULL) {
        visit(funDec->varList);
    }

}

static void visitVarList(void *node) {
    VarList *varList = (VarList *)node;
    visit(varList->paramDec);
    if (varList->varList != NULL) {
        visit(varList->varList);
    }
}

static void visitParamDec(void *node) {
    ParamDec *paramDec = (ParamDec *)node;
    visit(paramDec->specifier);
    visit(paramDec->varDec);
}

static void visitCompSt(void *node) {
    CompSt *compSt = (CompSt *)node;
    visit(compSt->defList);
    visit(compSt->stmtList);
}

static void visitStmtList(void *node) {
    /* may produce epsilon */
    StmtList *stmtList = (StmtList *)node;
    if (stmtList->stmt != NULL) {
        // stmtList->stmt and stmtList->stmtList
        // will both be non-null
        visit(stmtList->stmt);
        visit(stmtList->stmtList);
    }
}

static void visitStmt(void *node) {
    Stmt *stmt = (Stmt *)node;
    translate_Stmt(stmt);
}

static void visitDefList(void *node) {
    /* may produce epsilon */
    DefList *defList = (DefList *)node;
    if (defList->def != NULL) {
        visit(defList->def);
        visit(defList->defList);
    }
}

static void visitDef(void *node) {
    Def *def = (Def *)node;
    visit(def->specifier);
    visit(def->decList);
}

static void visitDecList(void *node) {
    DecList *decList = (DecList *)node;
    visit(decList->dec);
    if (decList->decList != NULL) {
        visit(decList->decList);
    }
}

static void visitDec(void *node) {
    Dec *dec = (Dec *)node;
    visit(dec->varDec);
    if (dec->exp != NULL) {
        visit(dec->exp);
    }

}

static void visitExp(void *node) {
    Exp *exp = (Exp *)node;

    warn("should not visit Exp");

    if (exp->exp_kind == EXP_T_INFIX) {

        Exp *left = exp->infix.exp_left;
        Exp *right = exp->infix.exp_right;

        Operand *temp1 = newTemp();
        Operand *temp2 = newTemp();
        left->ir_addr = temp1;
        right->ir_addr = temp2;
        visit(left);
        visit(right);

        IR *ir;

        if (exp->infix.op == PLUS) {
            gen(newAdd(exp->ir_addr, temp1, temp2));
        } else if (exp->infix.op == MINUS) {
            gen(newSub(exp->ir_addr, temp1, temp2));
        } else if (exp->infix.op == STAR) {
            gen(newMul(exp->ir_addr, temp1, temp2));
        } else if (exp->infix.op == DIV) {
            gen(newDiv(exp->ir_addr, temp1, temp2));

        } else if (exp->infix.op == ASSIGNOP) {
            // we now assume that ID appears on the left
            gen(newAssign(left->ir_lvalue_addr, right->ir_addr));

        } else if (exp->infix.op == RELOP) {
            gen(newIf(left->ir_addr, exp->infix.op_yylval, right->ir_addr, exp->ir_true));
            gen(newGoto(exp->ir_false));

        } else {
            fatal("unknown exp->infix.op");
        }

    } else if (exp->exp_kind == EXP_T_PAREN) {
        visit(exp->paren.exp);

    } else if (exp->exp_kind == EXP_T_UNARY) {

        if (exp->unary.op == MINUS) {
            Operand *temp = newTemp();
            exp->unary.exp->ir_addr = temp;
            visit(exp->unary.exp);
            gen(newSub(exp->ir_addr, newIntLiteral(0), temp));
        } else {
            fatal("unknown exp->unary.op");
        }

    } else if (exp->exp_kind == EXP_T_CALL) {
        if (exp->call.args != NULL) {
            visit(exp->call.args);
        }
        if (strcmp(exp->call.id_text, "read") == 0) {
            gen(newRead(exp->ir_addr));
        } else {
            // TODO
            fatal("cannot deal function call");
        }

    } else if (exp->exp_kind == EXP_T_SUBSCRIPT) {
        visit(exp->subscript.array);
        visit(exp->subscript.index);

    } else if (exp->exp_kind == EXP_T_DOT) {
        visit(exp->dot.exp);

    } else if (exp->exp_kind == EXP_T_ID) {
        exp->ir_lvalue_addr = newVariableOperand();

    } else if (exp->exp_kind == EXP_T_INT) {
        gen(newAssignInt(exp->ir_addr, exp->int_value));

    } else if (exp->exp_kind == EXP_T_FLOAT) {
        gen(newAssignFloat(exp->ir_addr, exp->float_value));

    } else {
        fatal("unknown exp_type");
    }
}

static void visitArgs(void *node) {
    Args *args = (Args *)node;
    visit(args->exp);
    if (args->args != NULL) {
        visit(args->args);
    }
}

static funcptr visitor_table[] = {
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

static void visit(void *node) {
    int type = *(int *)node;
    visitor_table[type-400](node);
}

void generate_intercode() {
    info("generate intercode");
    visit(root);
}

void translate_Exp(Exp *exp, Operand *place) {

    if (exp->exp_kind == EXP_T_INFIX && exp->infix.op == RELOP
            || exp->exp_kind == EXP_T_INFIX && exp->infix.op == AND
            || exp->exp_kind == EXP_T_INFIX && exp->infix.op == OR
            || exp->exp_kind == EXP_T_UNARY && exp->unary.op == NOT) {
        Label *L1 = newLabel();
        Label *L2 = newLabel();
        gen(newAssignInt(place, 0));
        translate_Condition(exp, L1, L2);
        gen(newLabelIR(L1));
        gen(newAssignInt(place, 1));
        gen(newLabelIR(L2));

    } else if (exp->exp_kind == EXP_T_INFIX) {

        Exp *left = exp->infix.exp_left;
        Exp *right = exp->infix.exp_right;

        Operand *temp1 = newTemp();
        Operand *temp2 = newTemp();
        translate_Exp(left, temp1);
        translate_Exp(right, temp2);

        if (exp->infix.op == PLUS) {
            gen(newAdd(place, temp1, temp2));
        } else if (exp->infix.op == MINUS) {
            gen(newSub(place, temp1, temp2));
        } else if (exp->infix.op == STAR) {
            gen(newMul(place, temp1, temp2));
        } else if (exp->infix.op == DIV) {
            gen(newDiv(place, temp1, temp2));

        } else if (exp->infix.op == ASSIGNOP) {
            // we now assume that ID appears on the left
            gen(newAssign(left->ir_lvalue_addr, temp2));
            gen(newAssign(place, left->ir_lvalue_addr));

        } else {
            fatal("unknown exp->infix.op");
        }

    } else if (exp->exp_kind == EXP_T_PAREN) {
        translate_Exp(exp->paren.exp, place);

    } else if (exp->exp_kind == EXP_T_UNARY) {

        if (exp->unary.op == MINUS) {
            Operand *temp = newTemp();
            translate_Exp(exp->unary.exp, temp);
            gen(newSub(place, newIntLiteral(0), temp));
        } else {
            fatal("unknown exp->unary.op");
        }

    } else if (exp->exp_kind == EXP_T_CALL) {
        if (exp->call.args != NULL) {
            visit(exp->call.args);
        }
        if (strcmp(exp->call.id_text, "read") == 0) {
            gen(newRead(place));
        } else if (strcmp(exp->call.id_text, "write") == 0) {
            Exp *arg = exp->call.args->exp;
            assert(arg != NULL);
            Operand *temp = newTemp();
            translate_Exp(arg, temp);
            gen(newWrite(temp));
        } else {
            // TODO
            fatal("cannot deal function call");
        }

//    } else if (exp->exp_kind == EXP_T_SUBSCRIPT) {
//        visit(exp->subscript.array);
//        visit(exp->subscript.index);
//
//    } else if (exp->exp_kind == EXP_T_DOT) {
//        visit(exp->dot.exp);

    } else if (exp->exp_kind == EXP_T_ID) {
        Operand *var = getVariableOperand(exp->id_text);
        exp->ir_lvalue_addr = var;
        gen(newAssign(place, var));

    } else if (exp->exp_kind == EXP_T_INT) {
        gen(newAssignInt(place, exp->int_value));

    } else if (exp->exp_kind == EXP_T_FLOAT) {
        gen(newAssignFloat(place, exp->float_value));

    } else {
        fatal("unknown exp_type");
    }
}

void translate_Stmt(Stmt *stmt) {
    if (stmt->stmt_kind == STMT_T_EXP) {
        // assign a temp place for exp, but never use it
        Operand *temp = newTemp();
        translate_Exp(stmt->exp.exp, temp);

    } else if (stmt->stmt_kind == STMT_T_COMP_ST) {
        visit(stmt->compst.compSt);

    } else if (stmt->stmt_kind == STMT_T_RETURN) {
        Operand *temp = newTemp();
        translate_Exp(stmt->return_.exp, temp);
        gen(newReturn(temp));

    } else if (stmt->stmt_kind == STMT_T_IF) {
        Exp *B = stmt->if_.exp;
        Stmt *S1 = stmt->if_.then_stmt;
        Label *L1 = newLabel();
        Label *L2 = newLabel();
        translate_Condition(B, L1, L2);
        gen(newLabelIR(L1));
        translate_Stmt(S1);
        gen(newLabelIR(L2));

    } else if (stmt->stmt_kind == STMT_T_IF_ELSE) {
        Exp *B = stmt->ifelse.exp;
        Stmt *S1 = stmt->ifelse.then_stmt;
        Stmt *S2 = stmt->ifelse.else_stmt;
        Label *L1 = newLabel();
        Label *L2 = newLabel();
        Label *L3 = newLabel();
        translate_Condition(B, L1, L2);
        gen(newLabelIR(L1));
        translate_Stmt(S1);
        gen(newGoto(L3));
        gen(newLabelIR(L2));
        translate_Stmt(S2);
        gen(newLabelIR(L3));

    } else if (stmt->stmt_kind == STMT_T_WHILE) {
        Exp *B = stmt->while_.exp;
        Stmt *S1 = stmt->while_.stmt;
        Label *L1 = newLabel();
        Label *L2 = newLabel();
        Label *L3 = newLabel();
        gen(newLabelIR(L1));
        translate_Condition(B, L2, L3);
        gen(newLabelIR(L2));
        translate_Stmt(S1);
        gen(newGoto(L1));
        gen(newLabelIR(L3));

    } else {
        fatal("unknown stmt_type");
    }
}

static void translate_Condition(Exp *exp, Label *L_true, Label *L_false) {
    if (exp->exp_kind == EXP_T_INFIX && exp->infix.op == RELOP) {
        Operand *temp1 = newTemp();
        Operand *temp2 = newTemp();
        translate_Exp(exp->infix.exp_left, temp1);
        translate_Exp(exp->infix.exp_right, temp2);
        gen(newIf(temp1, exp->infix.op_yylval, temp2, L_true));
        gen(newGoto(L_false));

    } else if (exp->exp_kind == EXP_T_INFIX && exp->infix.op == AND) {
        Label *L1 = newLabel();
        translate_Condition(exp->infix.exp_left, L1, L_false);
        gen(newLabelIR(L1));
        translate_Condition(exp->infix.exp_right, L_true, L_false);

    } else if (exp->exp_kind == EXP_T_INFIX && exp->infix.op == OR) {
        Label *L1 = newLabel();
        translate_Condition(exp->infix.exp_left, L_true, L1);
        gen(newLabelIR(L1));
        translate_Condition(exp->infix.exp_right, L_true, L_false);

    } else if (exp->exp_kind == EXP_T_UNARY && exp->unary.op == NOT) {
        // switch true label and false label
        // TODO
        translate_Condition(exp->unary.exp, L_false, L_true);

    } else if (exp->exp_kind == EXP_T_PAREN) {
        translate_Condition(exp->paren.exp, L_true, L_false);

    } else {
        Operand *temp = newTemp();
        translate_Exp(exp, temp);
        gen(newIf(temp, RELOP_NE, newIntLiteral(0), L_true));
        gen(newGoto(L_false));
    }
}
