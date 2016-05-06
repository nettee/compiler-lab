#ifndef __IR_H__
#define __IR_H__

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

typedef struct IRNode_ IRNode;

typedef struct IRList_ IRList;

#endif
