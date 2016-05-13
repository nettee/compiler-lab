#ifndef __IR_H__
#define __IR_H__

typedef struct Label_ Label;

Label *newLabel();

typedef struct {
    enum { 
        TEMP, 
        VAR_OPERAND,
        INT_LITERAL,
        FLOAT_LITERAL,
    } kind;
    union {
        int temp_no;
        int var_no;
        int int_value;
        float float_value;
    };
} Operand;

Operand *newTemp();
Operand *newVariableOperand();
Operand *getVariableOperand(char *name);
Operand *newIntLiteral(int value);
Operand *newFloatLiteral(float value);

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
IR *newAddr(Operand *result, Operand *arg1);
IR *newIndir(Operand *result, Operand *arg1);
IR *newAssignToAddr(Operand *result, Operand *arg1);
IR *newGoto(Label *label);
IR *newIf(Operand *arg1, int relop, Operand *arg2, Label *label);
IR *newReturn(Operand *arg1);
IR *newAlloc(Operand *var, int size);
IR *newArg(Operand *arg1);
IR *newCall(Operand *result, Operand *arg1);
IR *newParam(Operand *arg1);
IR *newRead(Operand *arg1);
IR *newWrite(Operand *arg1);

#endif
