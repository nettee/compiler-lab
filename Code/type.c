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

bool isArrayType(Type *t) {
    return t->kind == ARRAY;
}

bool isEqvType(Type *t1, Type *t2) {
    if (t1->kind == ARBIT || t2->kind == ARBIT) {
        return true;
    }
    if (t1->kind == BASIC && t2->kind == BASIC) {
        return t1->basic == t2->basic;
    }
    return false;
}

bool isEqvTypeList(TypeNode *t1, TypeNode *t2) {
    while (t1 != NULL && t2 != NULL) {
        if (!isEqvType(t1->type, t2->type)) {
            return false;
        }
        t1 = t1->next;
        t2 = t2->next;
    }
    return t1 == NULL && t2 == NULL;
}

Type *newArbitType() {
    Type *type = malloc(sizeof(type));
    type->kind = ARBIT;

    return type;
}

Type *newBasicInt() {
    return newBasicType(T_INT);
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

Type *getElementType(Type *arrayType) {
    if (arrayType->kind != ARRAY) {
        fatal("not an array type");
    }
    return arrayType->array.elementType;
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
        Type *t = type;
        while (t->kind == ARRAY) {
            t = t->array.elementType;
        }
        off += sprintf(str + off, "%s", typeRepr(t));
        t = type;
        while (t->kind == ARRAY) {
            off += sprintf(str + off, "[%d]", t->array.length);
            t = t->array.elementType;
        }
    } else if (type->kind == STRUCTURE) {
        off += sprintf(str + off, "structure");
    } else {
        fatal("Unknown type kind");
    }
    return str;
}

char *typeListRepr(TypeNode *typeList) {
    char *str = malloc(100);
    memset(str, 0, 100);
    int off = 0;
    off += sprintf(str + off, "(");
    for (TypeNode *q = typeList; q != NULL; q = q->next) {
        off += sprintf(str + off, "%s", typeRepr(q->type));
        if (q->next != NULL) {
            off += sprintf(str + off, ", ");
        }
    }
    off += sprintf(str + off, ")");
    return str;
}

