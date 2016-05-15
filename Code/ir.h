#ifndef __IR_H__
#define __IR_H__

typedef struct Label_ Label;

Label *newLabel();

typedef struct Operand_ Operand;

struct Operand_ {
    enum { 
        TEMP, 
        SYM_OPERAND,
        VAR_OPERAND,
        INT_LITERAL,
        FLOAT_LITERAL,
        ADDR,
        INDIR,
    } kind;
    union {
        int temp_no;
        char *sym_name;
        char *var_name;
        int int_value;
        float float_value;
        Operand *addr_var;
        Operand *indir_var;
    };
};

Operand *newTemp();
Operand *newVariableOperand(char *name);
Operand *newIntLiteral(int value);
Operand *newFloatLiteral(float value);
Operand *newAddr(Operand *var);
Operand *newIndir(Operand *indir);

typedef struct IR_ IR;

char *ir_repr(IR *ir);

IR *newLabelIR(Label *label);
IR *newFunction(char *name);
IR *newAssignInt(Operand *result, int value);
IR *newAssignFloat(Operand *result, float value);
IR *newAssign(Operand *result, Operand *arg1);
IR *newAdd(Operand *result, Operand *arg1, Operand *arg2);
IR *newSub(Operand *result, Operand *arg1, Operand *arg2);
IR *newMul(Operand *result, Operand *arg1, Operand *arg2);
IR *newDiv(Operand *result, Operand *arg1, Operand *arg2);
IR *newGoto(Label *label);
IR *newIf(Operand *arg1, int relop, Operand *arg2, Label *label);
IR *newReturn(Operand *arg1);
IR *newAlloc(Operand *var, int size);
IR *newArg(Operand *arg1);
IR *newCall(Operand *result, char *name);
IR *newParam(char *name);
IR *newRead(Operand *arg1);
IR *newWrite(Operand *arg1);

#endif
