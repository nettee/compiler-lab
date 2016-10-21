#ifndef __REG_H__
#define __REG_H__

#include "common.h"
#include "ir.h"

typedef struct Reg_ Reg;

int getOffset(Operand *op);
Reg *getReg(Operand *op);
char *reg_repr(Reg *reg);

#endif
