#include "common.h"
#include "syntax.tab.h"
#include "pt.h"
#include "st.h"

#include "ir.h"

char *relop_repr(int relop);

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

Operand *newTemp() {
    new_op(temp, TEMP);
    temp->temp_no = ++nr_temp;
    return temp;
}

Operand *newVariableOperand() {
    static int nr_variable = 0;
    new_op(var, VAR_OPERAND);
    var->var_no = ++nr_variable;
    return var;
}

Operand *getVariableOperand(char *name) {
    int var_no = retrieve_variable_rank(name);
    new_op(var, VAR_OPERAND);
    var->var_no = var_no;
    return var;
}

Operand *newIntLiteral(int value) {
    new_op(o, INT_LITERAL);
    o->int_value = value;
    return o;
}

Operand *newFloatLiteral(float value) {
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

struct Label_ {
    int label_no;
};

Label *newLabel() {
    static int nr_label = 0;
    Label *l = malloc(sizeof(Label));
    l->label_no = ++ nr_label;
    return l;
}

static char *label_repr(Label *label) {
    char *str = malloc(100);
    memset(str, 0, 100);
    int off = 0;
    off += sprintf(str + off, "L%d", label->label_no);
    return str;
}

struct IR_ {
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
        IR_IF,
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
        } function;
        // for LABEL
        struct {
            int label_no;
        } label;
        // for IF
        struct {
            Operand *arg1;
            Operand *arg2;
            int relop;
            Label *label;
        } if_;
        // for GOTO
        struct {
            Label *label;
        } goto_;
    };
};

char *ir_repr(IR *ir) {
    char *str = malloc(100);
    memset(str, 0, 100);
    int off = 0;
    if (ir->kind == IR_LABEL) {
        off += sprintf(str + off, "LABEL L%d :", ir->label.label_no);
    } else if (ir->kind == IR_FUNCTION) {
        off += sprintf(str + off, "FUNCTION %s :", ir->function.name);
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
    } else if (ir->kind == IR_GOTO) {
        off += sprintf(str + off, "GOTO %s", 
                label_repr(ir->goto_.label));
    } else if (ir->kind == IR_IF) {
        off += sprintf(str + off, "IF %s %s %s GOTO %s",
                op_repr(ir->if_.arg1),
                relop_repr(ir->if_.relop),
                op_repr(ir->if_.arg2),
                label_repr(ir->if_.label));
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

IR *newLabelIR(Label *label) {
    new_ir(ir, IR_LABEL);
    ir->label.label_no = label->label_no;
    return ir;
}

IR *newFunction(char *name) {
    new_ir(ir, IR_FUNCTION);
    ir->function.name = name;
    return ir;
}

IR *newAssignInt(Operand *result, int value) {
    new_ir(ir, IR_ASSIGN);
    ir->result = result;
    ir->arg1 = newIntLiteral(value);
    return ir;
}

IR *newAssignFloat(Operand *result, float value) {
    new_ir(ir, IR_ASSIGN);
    ir->result = result;
    ir->arg1 = newFloatLiteral(value);
    return ir;
}

IR *newAssign(Operand *result, Operand *arg1) {
    new_ir(ir, IR_ASSIGN);
    ir->result = result;
    ir->arg1 = arg1;
    return ir;
}

IR *newAdd(Operand *result, Operand *arg1, Operand *arg2) {
    new_ir(ir, IR_ADD);
    ir->result = result;
    ir->arg1 = arg1;
    ir->arg2 = arg2;
    return ir;
}

IR *newSub(Operand *result, Operand *arg1, Operand *arg2) {
    new_ir(ir, IR_SUB);
    ir->result = result;
    ir->arg1 = arg1;
    ir->arg2 = arg2;
    return ir;
}

IR *newMul(Operand *result, Operand *arg1, Operand *arg2) {
    new_ir(ir, IR_MUL);
    ir->result = result;
    ir->arg1 = arg1;
    ir->arg2 = arg2;
    return ir;
}

IR *newDiv(Operand *result, Operand *arg1, Operand *arg2) {
    new_ir(ir, IR_DIV);
    ir->result = result;
    ir->arg1 = arg1;
    ir->arg2 = arg2;
    return ir;
}

IR *newGoto(Label *label) {
    new_ir(ir, IR_GOTO);
    ir->goto_.label = label;
    return ir;
}

IR *newIf(Operand *arg1, int relop, Operand *arg2, Label *label) {
    new_ir(ir, IR_IF);
    ir->if_.arg1 = arg1;
    ir->if_.relop = relop;
    ir->if_.arg2 = arg2;
    ir->if_.label = label;
    return ir;
}

IR *newReturn(Operand *arg1) {
    new_ir(ir, IR_RETURN);
    ir->arg1 = arg1;
    return ir;
}

IR *newRead(Operand *temp) {
    new_ir(ir, IR_READ);
    ir->arg1 = temp;
    return ir;
}

IR *newWrite(Operand *temp) {
    new_ir(ir, IR_WRITE);
    ir->arg1 = temp;
    return ir;
}
//
//struct IRNode_ {
//    IRNode *prev;
//    IRNode *next;
//    IR *ir;
//};
//
//struct IRList_ {
//    IRNode *head;
//    IRNode *tail;
//};
//
//static IRList *IR2List(IR *ir) {
//    new_ir_node(node, ir);
//    IRList *irList = malloc(sizeof(IRList));
//    irList->head = node;
//    irList->tail = node;
//    return irList;
//}
//
//static IRList *newEmptyIRList() {
//    IRList *irList = malloc(sizeof(IRList));
//    irList->head = NULL;
//    irList->tail = NULL;
//    return irList;
//}
//
//static bool isEmptyIRList(IRList *list) {
//    return list->head == NULL && list->tail == NULL;
//}
//
//static IRList *IRList_extend(IRList *a, IRList *b) {
//    if (isEmptyIRList(a)) {
//        return b;
//    }
//    if (isEmptyIRList(b)) {
//        return a;
//    }
//    a->tail->next = b->head;
//    b->head->prev = a->tail;
//    IRList *irList = malloc(sizeof(IRList));
//    irList->head = a->head;
//    irList->tail = b->tail;
//    return irList;
//}
//
//static IRList *IRList_append(IRList *irList, IR *ir) {
//    if (isEmptyIRList(irList)) {
//        return IR2List(ir);
//    }
//    new_ir_node(node, ir);
//    irList->tail->next = node;
//    node->prev = irList->tail;
//    IRList *newIrList = malloc(sizeof(IRList));
//    newIrList->head = irList->head;
//    newIrList->tail = node;
//    return newIrList;
//}
//
//static void print_ir_list(IRList *irList) {
//    if (isEmptyIRList(irList)) {
//        return;
//    }
//    IRNode *q = irList->head;
//    while (true) {
//        printf("%s\n", ir_repr(q->ir));
//        if (q == irList->tail) {
//            break;
//        }
//        q = q->next;
//    }
//}
