#include "common.h"
#include "ir.h"

#include "asm.h"

extern IRList irList;

#define print0(...) { \
    printf(__VA_ARGS__); \
    printf("\n"); \
}

#define print(...) { \
    printf("  "); \
    print0(__VA_ARGS__); \
}

static char *reg(Operand *op) {
    return op_repr(op);
}

static void translate_label(IR *ir) {
    print0("L%d:", ir->label.label_no);
}

static void translate_function(IR *ir) {
//    printf("  %s\n", ir_repr(ir));
}

static void translate_assign(IR *ir) {
    printf("  %s\n", ir_repr(ir));
    print("move %s, %s", reg(ir->result), reg(ir->arg1));
}

static void translate_add(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_sub(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_mul(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_div(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_goto(IR *ir) {
    print("j %s", label_repr(ir->goto_.label));
}

static void translate_if(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_return(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_alloc(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_arg(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_call(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_param(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_read(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_write(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

typedef void (*funcptr)(IR *ir);

static funcptr translate_func_table[] = {
    translate_label,
    translate_function,
    translate_assign,
    translate_add,
    translate_sub,
    translate_mul,
    translate_div,
    translate_goto,
    translate_if,
    translate_return,
    translate_alloc,
    translate_arg,
    translate_call,
    translate_param,
    translate_read,
    translate_write,
};

static void translate_IR(IR *ir) {
    translate_func_table[ir->kind](ir);
}

void generate_asm() {
    info("generating asm...");
    for (IRNode *q = irList.head; q != NULL; q = q->next) {
        IR *ir = q->ir;
        translate_IR(ir);
    }
}
