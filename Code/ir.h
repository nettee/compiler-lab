#ifndef __IR_H__
#define __IR_H__

typedef struct {
    enum { 
        TEMP, 
        INT_LITERAL,
        FLOAT_LITERAL,
    } kind;
    union {
        int temp_no;
        int int_value;
        float float_value;
    };
} Operand;

typedef struct IRNode_ IRNode;

typedef struct IRList_ IRList;

#endif
