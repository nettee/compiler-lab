#include "reg.h"

struct Reg_ {
    Operand *op;
};

static Reg *newReg(Operand *op) {
    Reg *reg = malloc(sizeof(Reg));
    reg->op = op;
    return reg;
}

char *reg_repr(Reg *reg) {
    char *str = malloc(100);
    memset(str, 0, 100);
    sprintf(str, "r_%s", op_repr(reg->op));
    return str;
}

Reg *getReg(Operand *op) {
    Reg *reg = newReg(op);
    return reg;
}

int getOffset(Operand *op) {
    return -4;
}
