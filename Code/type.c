#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "type.h"

bool isArbitType(Type *t) {
    return t->kind == ARBIT;
}

bool isBasicType(Type *t) {
    return t->kind == BASIC;
}

bool isBasicIntType(Type *t) {
    return isArbitType(t) ||
        (isBasicType(t) && t->basic == T_INT);
}

bool isArrayType(Type *t) {
    return isArbitType(t) || t->kind == ARRAY;
}

bool isEqvType(Type *t1, Type *t2) {
    if (isArbitType(t1) || isArbitType(t2)) {
        return true;
    }
    if (isBasicType(t1) && isBasicType(t2)) {
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

Type *getArbitType() {
    static Type *type;
    if (type == NULL) {
        type = malloc(sizeof(type));
        type->kind = ARBIT;
    }

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
    if (isArbitType(arrayType)) {
        return getArbitType();
    }
    if (arrayType->kind != ARRAY) {
        fatal("not an array type");
    }
    return arrayType->array.elementType;
}

Type *newStructureType(char *name, FieldNode *fieldList) {
    Type *type = malloc(sizeof(Type));
    type->kind = STRUCTURE;
    type->structure.name = name;
    type->structure.fieldList = fieldList;

    return type;
}

FieldNode *newFieldNode(char *name, Type *type) {
    FieldNode *field = malloc(sizeof(FieldNode));
    field->next = NULL;
    field->name = name;
    field->type = type;
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
        off += sprintf(str + off, "%s", 
                (type->structure.name == NULL)
                ? "<anonymous>"
                : type->structure.name);
        off += sprintf(str + off, "{");
        for (FieldNode *q = type->structure.fieldList;
                q != NULL; q = q->next) {
            off += sprintf(str + off, "%s : %s",
                    q->name, typeRepr(q->type));
            if (q->next != NULL) {
                off += sprintf(str + off, ", ");
            }
        }
        off += sprintf(str + off, "}");
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

