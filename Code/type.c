#include <stdio.h>
#include <stdlib.h>

#include "type.h"

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
    printf("}\n");
}

