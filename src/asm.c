#include "common.h"
#include "ir.h"
#include "reg.h"
#include "st.h"
#include "syntax.tab.h"

extern IRList irList;
extern FILE *asm_out;

#define mips0(...) { \
    printf(__VA_ARGS__); \
    printf("\n"); \
    fprintf(asm_out, __VA_ARGS__); \
    fprintf(asm_out, "\n"); \
}

#define mips(...) { \
    printf("  "); \
    fprintf(asm_out, "  "); \
    mips0(__VA_ARGS__); \
}

#define li(reg, i) \
    mips("li %s, %d", reg, i);
#define la(reg, var) \
    mips("la %s, %s", reg, var_repr(var))
#define load(reg, var) \
    mips("lw %s, %s", reg, var_repr(var))
#define store(reg, var) \
    mips("sw %s, %s", reg, var_repr(var))
#define load_indir(reg, var) { \
    load(t3, var); \
    mips("lw %s, 0(%s)", reg, t3); \
}
#define store_indir(reg, var) { \
    load(t3, var); \
    mips("sw %s, 0(%s)", reg, t3); \
}
#define push(reg) { \
    mips("addi $sp, $sp, -4"); \
    mips("sw %s, 0($sp)", reg); \
}
#define pop(reg) { \
    mips("lw %s, 0($sp)", reg); \
    mips("addi $sp, $sp, 4"); \
}


#define t0 "$t0"
#define t1 "$t1"
#define t2 "$t2"
#define t3 "$t3"
#define a0 "$a0"
#define v0 "$v0"
#define ra "$ra"

int arg_cnt = 0;
int param_cnt = 0;

static void load_to(char *reg, Operand *op) {
    if (op->kind == INT_LITERAL) {
        li(reg, op->int_value);
    } else if (op->kind == ADDR) {
        la(reg, op->addr_var);
    } else if (op->kind == INDIR) {
        load_indir(reg, op->indir_var);
    } else {
        load(reg, op);
    }
}

static void translate_label(IR *ir) {
    mips0("L%d:", ir->label.label_no);
}

static void translate_function(IR *ir) {
    mips("jr $ra"); // deal with previous no return function
    mips0("  ");
    if (strcmp(ir->function.name, "main") == 0) {
        mips0("main:");
    } else {
        mips0("func_%s:", ir->function.name);
    }
    param_cnt = 0;
}

static void translate_assign(IR *ir) {
    load_to(t1, ir->arg1);
    mips("move %s, %s", t0, t1);
    if (ir->result->kind == INDIR) {
        store_indir(t0, ir->result->indir_var);
    } else {
        store(t0, ir->result);
    }
}

static void translate_add(IR *ir) {
    load_to(t1, ir->arg1);
    load_to(t2, ir->arg2);
    mips("add %s, %s, %s", t0, t1, t2);
    store(t0, ir->result);
}

static void translate_sub(IR *ir) {
    load_to(t1, ir->arg1);
    load_to(t2, ir->arg2);
    mips("sub %s, %s, %s", t0, t1, t2);
    store(t0, ir->result);
}

static void translate_mul(IR *ir) {
    load_to(t1, ir->arg1);
    load_to(t2, ir->arg2);
    mips("mul %s, %s, %s", t0, t1, t2);
    store(t0, ir->result);
}

static void translate_div(IR *ir) {
    load_to(t1, ir->arg1);
    load_to(t2, ir->arg2);
    mips("div %s, %s", t1, t2);
    mips("mflo %s", t0);
    store(t0, ir->result);
}

static void translate_goto(IR *ir) {
    mips("j %s", label_repr(ir->goto_.label));
}

char *break_repr(int relop) {
    if (relop == RELOP_LT) {
        return "lt";
    } else if (relop == RELOP_LE) {
        return "le";
    } else if (relop == RELOP_GT) {
        return "gt";
    } else if (relop == RELOP_GE) {
        return "ge";
    } else if (relop == RELOP_EQ) {
        return "eq";
    } else if (relop == RELOP_NE) {
        return "ne";
    } else {
        fatal("unknown relop");
    }
}

static void translate_if(IR *ir) {
    load_to(t1, ir->if_.arg1);
    load_to(t2, ir->if_.arg2);
    mips("b%s %s, %s, %s", break_repr(ir->if_.relop),
            t1, t2, label_repr(ir->if_.label));
}

static void translate_return(IR *ir) {
    load_to(v0, ir->arg1);
    mips("jr $ra");
}

static void translate_alloc(IR *ir) {
    printf("  %s\n", ir_repr(ir));
}

static void translate_arg(IR *ir) {
    load_to(t0, ir->arg1);
    push(t0);
    arg_cnt++;
}

static void translate_call(IR *ir) {
    push(ra);
    mips("jal func_%s", op_repr(ir->arg1));
    pop(ra);
    store(v0, ir->result);
    // remove pushed arguments
    mips("addi $sp, $sp, %d", 4 * arg_cnt);
    arg_cnt = 0;
}

static void translate_param(IR *ir) {
    param_cnt++;
    // move arguments in stack to static area
    mips("lw %s, %d($sp)", t0, 4 * param_cnt);
    store(t0, ir->arg1);
}

static void translate_read(IR *ir) {
    push(ra);
    mips("jal read");
    pop(ra);

    // get result
    store(v0, ir->arg1);
}

static void translate_write(IR *ir) {
    // pass argument
    load_to(a0, ir->arg1);

    push(ra);
    mips("jal write");
    pop(ra);
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

static void generate_data() {
    mips0(".data");

    SymbolTable *vst = get_current_env()->vst;
    for (SymbolNode *q = vst->head; q != NULL; q = q->next) {
        Symbol *sym = q->symbol;
        mips0("var_%s: .space 4", sym->name);
    }

    extern int nr_temp;
    int *shadowed = malloc((nr_temp + 1) * sizeof(int));
    for (int i = 1; i <= nr_temp; i++) {
        shadowed[i] = false;
    }
    for (IRNode *q = irList.head; q != NULL; q = q->next) {
        IR *ir = q->ir;
        if (ir->kind == IR_ALLOC) {
            mips0("var_%s: .space %d",
                    op_repr(ir->alloc.var), 
                    ir->alloc.size);
            // assume that all DEC is for temp var
            int temp_no = ir->alloc.var->temp_no;
            shadowed[temp_no] = true;
        }
    }

    for (int i = 1; i <= nr_temp; i++) {
        if (shadowed[i]) {
            continue;
        }
        mips0("var_t%d: .space 4", i);
    }

    mips0("_newline: .asciiz \"\\n\"");
    mips0("_prompt: .asciiz \"Enter an integer: \"");
    mips0("  ");
    mips0(".globl main");
}

static void generate_func() {
    mips0(".text");
    mips0("  ");
    mips0("read:");
    mips("li $v0, 4 # print_string");
    mips("la $a0, _prompt");
    mips("syscall");
    mips("li $v0, 5 # read_int");
    mips("syscall");
    mips("# $v0 takes the result");
    mips("jr $ra");
    mips0("  ");
    mips0("write:");
    mips("li $v0, 1 # print_int");
    mips("# $a0 come in as argument");
    mips("syscall");
    mips("li $v0, 4 # print_string");
    mips("la $a0, _newline");
    mips("syscall");
    mips("move $v0, $0");
    mips("jr $ra");
}

void generate_asm() {
    info("generating mips...");
    generate_data();
    generate_func();
    for (IRNode *q = irList.head; q != NULL; q = q->next) {
        IR *ir = q->ir;
        translate_IR(ir);
    }
    mips("move $v0, $0");
    mips("jr $ra");
}

