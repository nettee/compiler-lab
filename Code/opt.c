#include "common.h"
#include "ir.h"

extern IRList irList;

static bool isConstantAssignment(IR *ir) {
    return ir->kind == IR_ASSIGN 
        && ir->arg1->kind == INT_LITERAL;
}

static void constant_folding()
{
    for (IRNode *q = irList.head; q != NULL; q = q->next) {
        IR *ir = q->ir;
        if (isConstantAssignment(ir)) {
            info("constant assignment: %s", ir_repr(ir));
            IRList_remove(q);
            break;
        }
    }
}

void optimize()
{
    constant_folding();
}
