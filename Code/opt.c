#include "common.h"
#include "ir.h"

extern IRList irList;

static bool isConstantAssignment(IR *ir) {
    return ir->kind == IR_ASSIGN 
        && ir->result->kind == TEMP
        && ir->arg1->kind == INT_LITERAL;
}

static int op_replace(Operand **dest, int temp_no, Operand *repl) {
    if ((*dest)->kind == TEMP && (*dest)->temp_no == temp_no) {
        *dest = repl;
        return 1;
    } else {
        return 0;
    }
}

static int ir_replace_operand(IR *ir, Operand *temp, Operand *repl) {
    if (temp->kind != TEMP) {
        warn("can only replace temp variables");
        return 0;
    }
    int temp_no = temp->temp_no;
    int cnt = 0;
    if (ir->kind == IR_ADD
            || ir->kind == IR_SUB
            || ir->kind == IR_MUL
            || ir->kind == IR_DIV) {
        cnt += op_replace(&ir->arg1, temp_no, repl);
        cnt += op_replace(&ir->arg2, temp_no, repl);
    } else if (ir->kind == IR_ASSIGN
            || ir->kind == IR_RETURN) {
        cnt += op_replace(&ir->arg1, temp_no, repl);
    } else if (ir->kind == IR_IF) {
        cnt += op_replace(&ir->if_.arg1, temp_no, repl);
        cnt += op_replace(&ir->if_.arg2, temp_no, repl);
    }
    return cnt;
}

static void fold_constant(IRNode *begin, IRNode *end) {
    for (IRNode *p = begin; p != end; p = p->next) {
        IR *ir = p->ir;
        if (isConstantAssignment(ir)) {
            Operand *temp = ir->result;
            Operand *repl = ir->arg1;
            info("replace '%s' with '%s'", op_repr(temp), op_repr(repl));
            for (IRNode *q = begin; q != end; q = q->next) {
                IR *ir = q->ir;
                ir_replace_operand(ir, temp, repl);
            }
        }
    }
}

static IR *ir_compute_constant(IR *ir) {
    if (ir->kind == IR_ADD
            || ir->kind == IR_SUB
            || ir->kind == IR_MUL
            || ir->kind == IR_DIV) {
        Operand *a = ir->arg1;
        Operand *b = ir->arg2;
        if (a->kind == INT_LITERAL && b->kind == INT_LITERAL) {
            if (ir->kind == IR_ADD) {
                return newAssignInt(ir->result, a->int_value + b->int_value);
            } else if (ir->kind == IR_SUB) {
                return newAssignInt(ir->result, a->int_value - b->int_value);
            } else if (ir->kind == IR_MUL) {
                return newAssignInt(ir->result, a->int_value * b->int_value);
            } else if (ir->kind == IR_DIV) {
                return newAssignInt(ir->result, a->int_value / b->int_value);
            } else {
                return NULL;
            }
        } else if (a->kind == INT_LITERAL) {
            if (a->int_value == 0 && ir->kind == IR_ADD) {
                return newAssign(ir->result, b);
            } else if (a->int_value == 0 && ir->kind == IR_MUL) {
                return newAssignInt(ir->result, 0);
            } else {
                return NULL;
            }
        } else if (b->kind == INT_LITERAL) {
            if (b->int_value == 0 && ir->kind == IR_ADD) {
                return newAssign(ir->result, b);
            } else if (b->int_value == 0 && ir->kind == IR_SUB) {
                return newAssign(ir->result, b);
            } else if (b->int_value == 0 && ir->kind == IR_MUL) {
                return newAssignInt(ir->result, 0);
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

static void compute_constant(IRNode *begin, IRNode *end) {
    for (IRNode *q = begin; q != end; q = q->next) {
        IR *ir = q->ir;
        IR *new_ir = ir_compute_constant(ir);
        if (new_ir != NULL) {
            info("compute constant: '%s'", ir_repr(ir));
            q->ir = new_ir;
        }
    }
}

void optimize_block(IRNode *begin, IRNode *end) {
    info("optimizing block...");
    for (IRNode *q = begin; q != end; q = q->next) {
        IR *ir = q->ir;
        printf("    %s\n", ir_repr(ir));
    }
    for (int _ = 0; _ < 5; _++) {
        fold_constant(begin, end);
        compute_constant(begin, end);
    }
    for (IRNode *q = begin; q != end; q = q->next) {
        IR *ir = q->ir;
        printf("    %s\n", ir_repr(ir));
    }
}

bool is_dead_ir(IR *ir) {
    if (ir->kind != IR_ASSIGN) {
        return false;
    }
    Operand *op = ir->result;
    for (IRNode *q = irList.head; q != NULL; q = q->next) {
        if (ir_contains(q->ir, op)) {
            return false;
        }
    }
    return true;
}

void eliminate_dead_code() {
    info("eliminating dead code...");
    for (IRNode *q = irList.head; q != NULL; q = q->next) {
        IR *ir = q->ir;
        if (is_dead_ir(ir)) {
            info("dead IR: %s", ir_repr(ir));
            IRList_remove(q);
        }
    }
}

void optimize() {
    // divide block
    int N = IRList_length();
    IRNode **leaders = malloc(N * sizeof(IRNode *));
    for (int i = 0; i < N; i++) {
        leaders[i] = NULL;
    }

    int j = 0;
    for (IRNode *q = irList.head; q != NULL; q = q->next) {
        IR *ir = q->ir;
        if (q == irList.head
                || (ir->kind == IR_LABEL
                && q->prev != NULL 
                && q->prev->ir->kind != IR_LABEL)) {
            leaders[j++] = q;
        }
    }
    int nr_block = j;

    // optimize each block
    for (int i = 0; i < nr_block; i++) {
        optimize_block(leaders[i], leaders[i+1]);
    }

    eliminate_dead_code();

}
