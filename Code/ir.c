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

int nr_temp = 0;

Operand *newTemp() {
    new_op(temp, TEMP);
    temp->temp_no = ++nr_temp;
    return temp;
}

Operand *newSymbolOperand(char *name) {
    new_op(sym, SYM_OPERAND);
    sym->sym_name = name;
    return sym;
}

Operand *newVariableOperand(char *name) {
    new_op(var, VAR_OPERAND);
    var->var_name = name;
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

Operand *newAddr(Operand *var) {
    new_op(o, ADDR);
    o->addr_var = var;
    return o;
}

Operand *newIndir(Operand *var) {
    new_op(o, INDIR);
    o->indir_var = var;
    return o;
}

char *op_repr(Operand *op) {
    if (op == NULL) {
        warn("op == NULL");
        return "__";
    }
    char *str = malloc(100);
    memset(str, 0, 100);
    int off = 0;

    if (op->kind == TEMP) {
        off += sprintf(str + off, "t%d", op->temp_no);
    } else if (op->kind == SYM_OPERAND) {
        off += sprintf(str + off, "%s", op->sym_name);
    } else if (op->kind == VAR_OPERAND) {
        off += sprintf(str + off, "%s", op->var_name);
    } else if (op->kind == INT_LITERAL) {
        off += sprintf(str + off, "#%d", op->int_value);
    } else if (op->kind == FLOAT_LITERAL) {
        off += sprintf(str + off, "#%.2f", op->float_value);
    } else if (op->kind == ADDR) {
        off += sprintf(str + off, "&%s", op_repr(op->addr_var));
    } else if (op->kind == INDIR) {
        off += sprintf(str + off, "*%s", op_repr(op->indir_var));
    } else {
        off += sprintf(str + off, "some-op");
    }

    return str;
}

char *var_repr(Operand *op) {
    if (op->kind != VAR_OPERAND && op->kind != TEMP) {
        warn("op '%s' is not variable or temp", op_repr(op));
        return op_repr(op);
    }
    char *str = malloc(100);
    memset(str, 0, 100);
    sprintf(str, "var_%s", op_repr(op));
    return str;
}

bool op_equals(Operand *a, Operand *b) {
    if (a->kind == TEMP) {
        return b->kind == TEMP
            && a->temp_no == b->temp_no;
    } else if (a->kind == SYM_OPERAND) {
        return b->kind == SYM_OPERAND
            && strcmp(a->sym_name, b->sym_name) == 0;
    } else if (a->kind == VAR_OPERAND) {
        return b->kind == VAR_OPERAND
            && strcmp(a->var_name, b->var_name) == 0;
    } else if (a->kind == INT_LITERAL) {
        return b->kind == INT_LITERAL
            && a->int_value == b->int_value;
    } else if (a->kind == FLOAT_LITERAL) {
        return b->kind == FLOAT_LITERAL
            && a->float_value == b->float_value;
    } else if (a->kind == ADDR) {
        return b->kind == ADDR
            && op_equals(a->addr_var, b->addr_var);
    } else if (a->kind == INDIR) {
        return b->kind == INDIR
            && op_equals(a->indir_var, b->indir_var);
    } else {
        return false;
    }
}

bool op_contains(Operand *a, Operand *b) {
    if (op_equals(a, b)) {
        return true;
    } 
    if (a->kind == ADDR 
            && op_contains(a->addr_var, b)) {
        return true;
    } 
    if (a->kind == INDIR
            && op_contains(a->indir_var, b)) {
        return true;
    }
    return false;
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

char *label_repr(Label *label) {
    char *str = malloc(100);
    memset(str, 0, 100);
    int off = 0;
    off += sprintf(str + off, "L%d", label->label_no);
    return str;
}

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
    } else if (ir->kind == IR_ALLOC) {
        off += sprintf(str + off, "DEC %s %d", 
                op_repr(ir->alloc.var),
                ir->alloc.size);
    } else if (ir->kind == IR_ARG) {
        off += sprintf(str + off, "ARG %s", op_repr(ir->arg1));
    } else if (ir->kind == IR_CALL) {
        off += sprintf(str + off, "%s := CALL %s", 
                op_repr(ir->result),
                op_repr(ir->arg1));
    } else if (ir->kind == IR_PARAM) {
        off += sprintf(str + off, "PARAM %s", op_repr(ir->arg1));
    } else if (ir->kind == IR_READ) {
        off += sprintf(str + off, "READ %s", op_repr(ir->arg1));
    } else if (ir->kind == IR_WRITE) {
        off += sprintf(str + off, "WRITE %s", op_repr(ir->arg1));
    } else {
        off += sprintf(str + off, "some-ir");
    }
    return str;
}

bool ir_contains(IR *ir, Operand *op) {
    // return true if there exist read of op in ir
    if (ir->kind == IR_LABEL) {
        return false;
    } else if (ir->kind == IR_FUNCTION) {
        return false;
    } else if (ir->kind == IR_ASSIGN) {
        return op_contains(ir->arg1, op);
    } else if (ir->kind == IR_ADD) {
        return op_contains(ir->arg1, op) || op_contains(ir->arg2, op);
    } else if (ir->kind == IR_SUB) {
        return op_contains(ir->arg1, op) || op_contains(ir->arg2, op);
    } else if (ir->kind == IR_MUL) {
        return op_contains(ir->arg1, op) || op_contains(ir->arg2, op);
    } else if (ir->kind == IR_DIV) {
        return op_contains(ir->arg1, op) || op_contains(ir->arg2, op);
    } else if (ir->kind == IR_GOTO) {
        return false;
    } else if (ir->kind == IR_IF) {
        return op_contains(ir->if_.arg1, op) 
            || op_contains(ir->if_.arg2, op);
    } else if (ir->kind == IR_RETURN) {
        return op_contains(ir->arg1, op);
    } else if (ir->kind == IR_ALLOC) {
        return op_contains(ir->alloc.var, op);
    } else if (ir->kind == IR_ARG) {
        return op_contains(ir->arg1, op);
    } else if (ir->kind == IR_CALL) {
        return false;
    } else if (ir->kind == IR_PARAM) {
        return false;
    } else if (ir->kind == IR_READ) {
        return false;
    } else if (ir->kind == IR_WRITE) {
        return op_contains(ir->arg1, op);
    } else {
        return false;
    }
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

IR *newAlloc(Operand *var, int size) {
    new_ir(ir, IR_ALLOC);
    ir->alloc.var = var;
    ir->alloc.size = size;
    return ir;
}

IR *newArg(Operand *arg1) {
    new_ir(ir, IR_ARG);
    ir->arg1 = arg1;
    return ir;
}

IR *newCall(Operand *result, char *name) {
    Operand *sym = newSymbolOperand(name);
    new_ir(ir, IR_CALL);
    ir->result = result;
    ir->arg1 = sym;
    return ir;
}


IR *newParam(char *name) {
    Operand *var = newVariableOperand(name);
    new_ir(ir, IR_PARAM);
    ir->arg1 = var;
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

extern FILE *ir_out_file;
extern FILE *ir_out_file2;
extern FILE *ir_out;

IRList irList;

void IRList_init() {
    irList.length = 0;
    irList.head = NULL;
    irList.tail = NULL;
}

int IRList_length() {
    return irList.length;
}

void IRList_add(IR *ir) {
    IRNode *irNode = malloc(sizeof(IRNode));
    irNode->prev = NULL;
    irNode->next = NULL;
    irNode->ir = ir;
    if (irList.head == NULL) {
        irList.head = irNode;
        irList.tail = irNode;
    } else {
        irList.tail->next = irNode;
        irNode->prev = irList.tail;
        irList.tail = irNode;
    }
    irList.length += 1;
}

void IRList_remove(IRNode *irNode) {
    if (irList.head == NULL) {
        fatal("remove from empty IRList");
        return;
    }
    if (irList.head == irNode && irList.tail == irNode) {
        irList.head = NULL;
        irList.tail = NULL;
    } else if (irList.head == irNode) {
        irNode->next->prev = NULL;
        irList.head = irNode->next;
    } else if (irList.tail == irNode) {
        irNode->prev->next = NULL;
        irList.tail = irNode->prev;
    } else {
        irNode->prev->next = irNode->next;
        irNode->next->prev = irNode->prev;
    }
    irList.length -= 1;
}

void IRList_print_to_file(FILE *file) {
    for (IRNode *q = irList.head; q != NULL; q = q->next) {
        IR *ir = q->ir;
        char *repr = ir_repr(ir);
//        printf("%s\n", repr);
        fprintf(file, "%s\n", repr);
    }
}

void IRList_print() {
    IRList_print_to_file(ir_out_file);
//    IRList_print_to_file(ir_out);
}

void IRList_print_2() {
    IRList_print_to_file(ir_out_file2);
}
