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

char *op_repr(Operand *op);
bool op_equals(Operand *, Operand *);
bool op_contains(Operand *, Operand *);

typedef struct IR_ IR;

struct IR_ {
    enum {
        IR_LABEL,
        IR_FUNCTION,
        IR_ASSIGN,
        IR_ADD, 
        IR_SUB, 
        IR_MUL, 
        IR_DIV,
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
         * RETURN, ARG, CALL, PARAM,
         * READ, WRITE
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
        // for ALLOC(DEC)
        struct {
            Operand *var;
            int size;
        } alloc;
    };
};

char *ir_repr(IR *ir);
bool ir_contains(IR *ir, Operand *op);

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

typedef struct IRNode_ IRNode;

struct IRNode_ {
    IRNode *prev;
    IRNode *next;
    IR *ir;
};

typedef struct {
    int length;
    IRNode *head;
    IRNode *tail;
} IRList;

void IRList_init();
int IRList_length();
void IRList_add(IR *ir);
void IRList_remove(IRNode *irNode);
void IRList_print();
void IRList_print_2();

#endif
