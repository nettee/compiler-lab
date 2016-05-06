#include "common.h"
#include "syntax.tab.h"
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

static int nr_temp = 0;

static Operand *newTemp() {
    new_op(temp, TEMP);
    temp->temp_no = ++nr_temp;
    return temp;
}

static Operand *newVariableOperand() {
    static int nr_variable = 0;
    new_op(var, VAR_OPERAND);
    var->var_no = ++nr_variable;
    return var;
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
    } else if (op->kind == VAR_OPERAND) {
        off += sprintf(str + off, "v%d", op->var_no);
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
        IR_LABEL,
        IR_FUNCTION,
        IR_ASSIGN,
        IR_ADD, 
        IR_SUB, 
        IR_MUL, 
        IR_DIV,
        IR_ADDR,
        IR_INDIR,
        IR_ASSIGN_TO_ADDR,
        IR_GOTO,
        IR_COND_GOTO,
        IR_RETURN,
        IR_ALLOC,
        IR_ARG,
        IR_CALL,
        IR_PARAM,
        IR_READ,
        IR_WRITE,
    } kind;
    union {
        /* for ASSIGN, ADD, SUB, MUL, DIV,
         * RETURN,
         */
        struct {
            Operand *result;
            Operand *arg1;
            Operand *arg2;
        };
        // for FUNCTION
        struct {
            char *name;
        };
    };
} IR;

static char *ir_repr(IR *ir) {
    char *str = malloc(100);
    memset(str, 0, 100);
    int off = 0;
    if (ir->kind == IR_LABEL) {
        // TODO
    } else if (ir->kind == IR_FUNCTION) {
        off += sprintf(str + off, "FUNCTION %s :", ir->name);
    } else if (ir->kind == IR_ASSIGN) {
        off += sprintf(str + off, "%s := %s",
                op_repr(ir->result),
                op_repr(ir->arg1));
    } else if (ir->kind == IR_ADD) {
        off += sprintf(str + off, "%s := %s + %s",
                op_repr(ir->result),
                op_repr(ir->arg1),
                op_repr(ir->arg2));
    } else if (ir->kind == IR_SUB) {
        off += sprintf(str + off, "%s := %s - %s",
                op_repr(ir->result),
                op_repr(ir->arg1),
                op_repr(ir->arg2));
    } else if (ir->kind == IR_MUL) {
        off += sprintf(str + off, "%s := %s * %s",
                op_repr(ir->result),
                op_repr(ir->arg1),
                op_repr(ir->arg2));
    } else if (ir->kind == IR_DIV) {
        off += sprintf(str + off, "%s := %s / %s",
                op_repr(ir->result),
                op_repr(ir->arg1),
                op_repr(ir->arg2));
    } else if (ir->kind == IR_RETURN) {
        off += sprintf(str + off, "RETURN %s", op_repr(ir->arg1));
    } else if (ir->kind == IR_READ) {
        off += sprintf(str + off, "READ %s", op_repr(ir->arg1));
    } else if (ir->kind == IR_WRITE) {
        off += sprintf(str + off, "WRITE %s", op_repr(ir->arg1));
    } else {
        off += sprintf(str + off, "some-ir");
    }
    return str;
}

static IR *newFunction(char *name) {
    new_ir(ir, IR_FUNCTION);
    ir->name = name;
    return ir;
}

static IR *newAssignInt(Operand *result, int value) {
    new_ir(ir, IR_ASSIGN);
    ir->result = result;
    ir->arg1 = newIntLiteral(value);
    return ir;
}

static IR *newAssignFloat(Operand *result, float value) {
    new_ir(ir, IR_ASSIGN);
    ir->result = result;
    ir->arg1 = newFloatLiteral(value);
    return ir;
}

static IR *newAssign(Operand *result, Operand *arg1) {
    new_ir(ir, IR_ASSIGN);
    ir->result = result;
    ir->arg1 = arg1;
    return ir;
}

static IR *newAdd(Operand *result, Operand *arg1, Operand *arg2) {
    new_ir(ir, IR_ADD);
    ir->result = result;
    ir->arg1 = arg1;
    ir->arg2 = arg2;
    return ir;
}

static IR *newSub(Operand *result, Operand *arg1, Operand *arg2) {
    new_ir(ir, IR_SUB);
    ir->result = result;
    ir->arg1 = arg1;
    ir->arg2 = arg2;
    return ir;
}

static IR *newMul(Operand *result, Operand *arg1, Operand *arg2) {
    new_ir(ir, IR_MUL);
    ir->result = result;
    ir->arg1 = arg1;
    ir->arg2 = arg2;
    return ir;
}

static IR *newDiv(Operand *result, Operand *arg1, Operand *arg2) {
    new_ir(ir, IR_DIV);
    ir->result = result;
    ir->arg1 = arg1;
    ir->arg2 = arg2;
    return ir;
}

static IR *newReturn(Operand *temp) {
    new_ir(ir, IR_RETURN);
    ir->arg1 = temp;
    return ir;
}

static IR *newRead(Operand *temp) {
    new_ir(ir, IR_READ);
    ir->arg1 = temp;
    return ir;
}

static IR *newWrite(Operand *temp) {
    new_ir(ir, IR_WRITE);
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

static IRList *newEmptyIRList() {
    IRList *irList = malloc(sizeof(IRList));
    irList->head = NULL;
    irList->tail = NULL;
    return irList;
}

static bool isEmptyIRList(IRList *list) {
    return list->head == NULL && list->tail == NULL;
}

static IRList *IRList_extend(IRList *a, IRList *b) {
    if (isEmptyIRList(a)) {
        return b;
    }
    if (isEmptyIRList(b)) {
        return a;
    }
    a->tail->next = b->head;
    b->head->prev = a->tail;
    IRList *irList = malloc(sizeof(IRList));
    irList->head = a->head;
    irList->tail = b->tail;
    return irList;
}

static IRList *IRList_append(IRList *irList, IR *ir) {
    if (isEmptyIRList(irList)) {
        return IR2List(ir);
    }
    new_ir_node(node, ir);
    irList->tail->next = node;
    node->prev = irList->tail;
    IRList *newIrList = malloc(sizeof(IRList));
    newIrList->head = irList->head;
    newIrList->tail = node;
    return newIrList;
}

static void print_ir_list(IRList *irList) {
    if (isEmptyIRList(irList)) {
        return;
    }
    IRNode *q = irList->head;
    while (true) {
        printf("%s\n", ir_repr(q->ir));
        if (q == irList->tail) {
            break;
        }
        q = q->next;
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
        IR *ir = newFunction(extDef->fun.funDec->id_text);
        extDef->ir_code = IRList_extend(IR2List(ir), 
                extDef->fun.compSt->ir_code);

        print_ir_list(extDef->ir_code);

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
    compSt->ir_code = compSt->stmtList->ir_code;
}

static void visitStmtList(void *node) {
    /* may produce epsilon */
    StmtList *stmtList = (StmtList *)node;
    if (stmtList->stmt == NULL) {
        stmtList->ir_code = newEmptyIRList();
    } else {
        // stmtList->stmt and stmtList->stmtList
        // will both be non-null
        visit(stmtList->stmt);
        visit(stmtList->stmtList);
        stmtList->ir_code = IRList_extend(
                stmtList->stmt->ir_code,
                stmtList->stmtList->ir_code);
    }
}

static void visitStmt(void *node) {
    Stmt *stmt = (Stmt *)node;
    if (stmt->stmt_kind == STMT_T_EXP) {
        /* assign a temp place for exp,
         * but never use it
         */
        Operand *temp = newTemp();
        stmt->exp.exp->ir_addr = temp;
        visit(stmt->exp.exp);
        stmt->ir_code = stmt->exp.exp->ir_code;

    } else if (stmt->stmt_kind == STMT_T_COMP_ST) {
        visit(stmt->compst.compSt);
        stmt->ir_code = stmt->compst.compSt->ir_code;

    } else if (stmt->stmt_kind == STMT_T_RETURN) {
        Operand *temp = newTemp();
        stmt->return_.exp->ir_addr = temp;
        visit(stmt->return_.exp);
        IR *ir = newReturn(temp);
        stmt->ir_code = IRList_append(stmt->return_.exp->ir_code, ir);

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
            ir = newAdd(exp->ir_addr, temp1, temp2);
        } else if (exp->infix.op == MINUS) {
            ir = newSub(exp->ir_addr, temp1, temp2);
        } else if (exp->infix.op == STAR) {
            ir = newMul(exp->ir_addr, temp1, temp2);
        } else if (exp->infix.op == DIV) {
            ir = newDiv(exp->ir_addr, temp1, temp2);

        } else if (exp->infix.op == ASSIGNOP) {
            // we not assume that ID appears on the left
            ir = newAssign(left->ir_lvalue_addr, right->ir_addr);

        } else {
            fatal("unknown exp->infix.op");
        }

        exp->ir_code = IRList_extend(left->ir_code,
                IRList_append(right->ir_code, ir));

    } else if (exp->exp_kind == EXP_T_PAREN) {
        visit(exp->paren.exp);

    } else if (exp->exp_kind == EXP_T_UNARY) {

        if (exp->unary.op == MINUS) {
            Operand *temp = newTemp();
            exp->unary.exp->ir_addr = temp;
            visit(exp->unary.exp);
            IR *ir = newSub(exp->ir_addr,
                    newIntLiteral(0), temp);
            exp->ir_code = IRList_append(
                    exp->unary.exp->ir_code, ir);
        } else {
            fatal("unknown exp->unary.op");
        }

    } else if (exp->exp_kind == EXP_T_CALL) {
        if (exp->call.args != NULL) {
            visit(exp->call.args);
        }
        if (strcmp(exp->call.id_text, "read") == 0) {
            IR *ir = newRead(exp->ir_addr);
            exp->ir_code = IR2List(ir);
        } else {
            fatal("cannot deal function call");
        }

    } else if (exp->exp_kind == EXP_T_SUBSCRIPT) {
        visit(exp->subscript.array);
        visit(exp->subscript.index);

    } else if (exp->exp_kind == EXP_T_DOT) {
        visit(exp->dot.exp);

    } else if (exp->exp_kind == EXP_T_ID) {
        exp->ir_lvalue_addr = newVariableOperand();
        exp->ir_code = newEmptyIRList();

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
