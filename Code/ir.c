#include "common.h"
#include "pt.h"

#include "ir.h"

#define new_op(o, kkind) \
    Operand * o = malloc(sizeof(Operand)); \
    o->kind = kkind
#define new_ir(ir, kkind) \
    IR * ir = malloc(sizeof(IR)); \
    ir->kind = kkind
#define new_ir_node(node, iir) \
    IRNode * node = malloc(sizeof(IRNode)); \
    node->prev = NULL; \
    node->next = NULL; \
    node->ir = iir


static Operand *newTemp() {
    static int n = 0;
    new_op(temp, TEMP);
    temp->temp_no = ++n;
    return temp;
}

static Operand *newIntLiteral(int value) {
    new_op(o, INT_LITERAL);
    o->int_value = value;
    return o;
}

static Operand *newFloatLiteral(float value) {
    new_op(o, FLOAT_LITERAL);
    o->float_value = value;
    return o;
}

static char *op_repr(Operand *op) {
    char *str = malloc(100);
    memset(str, 0, 100);
    int off = 0;

    if (op->kind == TEMP) {
        off += sprintf(str + off, "t%d", op->temp_no);
    } else if (op->kind == INT_LITERAL) {
        off += sprintf(str + off, "#%d", op->int_value);
    } else if (op->kind == FLOAT_LITERAL) {
        off += sprintf(str + off, "#%.2f", op->float_value);
    } else {
        off += sprintf(str + off, "some-op");
    }

    return str;
}

typedef struct {
    enum {
        LABEL,
        FUNCTION,
        ASSIGN,
        ADD, SUB, MUL, DIV,
        ADDR,
        INDIR,
        ASSIGN_TO_ADDR,
        GOTO,
        COND_GOTO,
        RETURN,
        ALLOC,
        ARG,
        CALL,
        PARAM,
        READ,
        WRITE,
    } kind;
    union {
        struct {
            Operand *result;
            Operand *arg1;
            Operand *arg2;
        };
    };
} IR;

static char *ir_repr(IR *ir) {
    char *str = malloc(100);
    memset(str, 0, 100);
    int off = 0;
    if (ir->kind == LABEL) {
        // TODO
    } else if (ir->kind == ASSIGN) {
        off += sprintf(str + off, "%s := %s", op_repr(ir->result), op_repr(ir->arg1));
    } else if (ir->kind == RETURN) {
        off += sprintf(str + off, "RETURN %s", op_repr(ir->arg1));
    } else {
        off += sprintf(str + off, "some-ir");
    }
    return str;
}

static IR *newAssignInt(Operand *result, int value) {
    new_ir(ir, ASSIGN);
    ir->result = result;
    ir->arg1 = newIntLiteral(value);
    return ir;
}

static IR *newAssignFloat(Operand *result, float value) {
    new_ir(ir, ASSIGN);
    ir->result = result;
    ir->arg1 = newFloatLiteral(value);
    return ir;
}

static IR *newReturn(Operand *temp) {
    new_ir(ir, RETURN);
    ir->arg1 = temp;
    return ir;
}

struct IRNode_ {
    IRNode *prev;
    IRNode *next;
    IR *ir;
};

struct IRList_ {
    IRNode *head;
    IRNode *tail;
};

static IRList *IR2List(IR *ir) {
    new_ir_node(node, ir);
    IRList *irList = malloc(sizeof(IRList));
    irList->head = node;
    irList->tail = node;
    return irList;
}

static IRList *IRList_append(IRList *irList, IR *ir) {
    new_ir_node(node, ir);
    irList->tail->next = node;
    node->prev = irList->tail;
    IRList *newIrList = malloc(sizeof(IRList));
    newIrList->head = irList->head;
    newIrList->tail = node;
    return newIrList;
}

static void print_ir_list(IRList *irList) {
    for (IRNode *q = irList->head; q != NULL; q = q->next) {
        printf("%s\n", ir_repr(q->ir));
    }
}

typedef void (*funcptr)(void *);

static void visit(void *node);

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
    if (stmt->stmt_kind == STMT_T_EXP) {
        visit(stmt->exp.exp);

    } else if (stmt->stmt_kind == STMT_T_COMP_ST) {
        visit(stmt->compst.compSt);

    } else if (stmt->stmt_kind == STMT_T_RETURN) {
        Operand *temp = newTemp();
        stmt->return_.exp->ir_addr = temp;
        visit(stmt->return_.exp);
        IR *ir = newReturn(temp);
        stmt->ir_code = IRList_append(stmt->return_.exp->ir_code, ir);
        print_ir_list(stmt->ir_code);

    } else if (stmt->stmt_kind == STMT_T_IF) {
        visit(stmt->if_.exp);
        visit(stmt->if_.then_stmt);

    } else if (stmt->stmt_kind == STMT_T_IF_ELSE) {
        visit(stmt->ifelse.exp);
        visit(stmt->ifelse.then_stmt);
        visit(stmt->ifelse.else_stmt);

    } else if (stmt->stmt_kind == STMT_T_WHILE) {
        visit(stmt->while_.exp);
        visit(stmt->while_.stmt);

    } else {
        fatal("unknown stmt_type");
    }
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

    if (exp->exp_kind == EXP_T_INFIX) {
        visit(exp->infix.exp_left);
        visit(exp->infix.exp_right);

    } else if (exp->exp_kind == EXP_T_PAREN) {
        visit(exp->paren.exp);

    } else if (exp->exp_kind == EXP_T_UNARY) {
        visit(exp->unary.exp);

    } else if (exp->exp_kind == EXP_T_CALL) {
        if (exp->call.args != NULL) {
            visit(exp->call.args);
        }

    } else if (exp->exp_kind == EXP_T_SUBSCRIPT) {
        visit(exp->subscript.array);
        visit(exp->subscript.index);

    } else if (exp->exp_kind == EXP_T_DOT) {
        visit(exp->dot.exp);

    } else if (exp->exp_kind == EXP_T_ID) {

    } else if (exp->exp_kind == EXP_T_INT) {
        IR *ir = newAssignInt(exp->ir_addr, exp->int_value);
        exp->ir_code = IR2List(ir);

    } else if (exp->exp_kind == EXP_T_FLOAT) {
        IR *ir = newAssignFloat(exp->ir_addr, exp->float_value);
        exp->ir_code = IR2List(ir);

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
