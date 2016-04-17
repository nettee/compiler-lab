#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "type.h"

bool isBasicIntType(Type *t) {
    return t->kind == BASIC && t->basic == T_INT;
}

bool isBasicType(Type *t) {
    return t->kind == BASIC;
}

bool isEqvType(Type *t1, Type *t2) {
    if (t1->kind == BASIC && t2->kind == BASIC) {
        return t1->basic == t2->basic;
    }
    return false;
}

Type *newBasicInt() {
     Type *p = newBasicType(T_INT);
     return p;
}

Type *newBasicFloat() {
    return newBasicType(T_FLOAT);
}

Type *newBasicType(int type_index) {
    Type *type = malloc(sizeof(Type));
    type->kind = BASIC;
    type->basic = type_index;

    return type;
}

Type *newArrayType(Type *elementType, int length) {
    Type *type = malloc(sizeof(Type));
    type->kind = ARRAY;
    type->array.elementType = elementType;
    type->array.length = length;

    return type;
}

static void printBasicType(int type_index) {
    switch (type_index) {
    case T_INT:
        printf("int");
        break;
    case T_FLOAT:
        printf("float");
        break;
    default:
        printf("\nFatal: Unknown specifier->type_value\n");
    }
}

void printType(Type *type) {
    printf("Type{");
    switch (type->kind) {
    case BASIC:
        printBasicType(type->basic);
        break;
    case ARRAY:
        printf("array");
        break;
    case STRUCTURE:
        printf("structure");
        break;
    default:
        printf("\nFatal: unknown type kind\n");
    }
    printf("}");
}

char *typeRepr(Type *type) {
    char *str = malloc(100);
    memset(str, 0, 100);
    int off = 0;
    if (type->kind == BASIC) {
        if (type->basic == T_INT) {
            off += sprintf(str + off, "int");
        } else if (type->basic == T_FLOAT) {
            off += sprintf(str + off, "float");
        } else {
            fatal("unknown type_index");
        }
    } else if (type->kind == ARRAY) {
        off += sprintf(str + off, "array");
    } else if (type->kind == STRUCTURE) {
        off += sprintf(str + off, "structure");
    } else {
        fatal("Unknown type kind");
    }
    return str;
}

