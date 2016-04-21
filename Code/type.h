#ifndef __TYPE_H__
#define __TYPE_H__

#include "common.h"

typedef int bool;
#define true 1
#define false 0

enum { T_INT, T_FLOAT };

typedef struct FieldNode_ FieldNode;
typedef struct Type_ Type;

struct Type_ {
    enum { BASIC, ARRAY, STRUCTURE, ARBIT } kind;
    union {
        int basic;
        struct {
            Type *elementType;
            int length;
        } array;
        struct {
            bool isAnony;
            FieldNode *fields;
        } structure;
    };
};

struct FieldNode_ {
    FieldNode *next;
    char *name;
    Type *type;
};

typedef struct TypeNode_ {
    struct TypeNode_ *next;
    Type *type;
} TypeNode;

bool isArbitType(Type *t);
bool isBasicIntType(Type *t);
bool isBasicType(Type *t);
bool isArrayType(Type *t);
bool isEqvType(Type *t1, Type *t2);
bool isEqvTypeList(TypeNode *t1, TypeNode *t2);

Type *newArbitType();
Type *getArbitType();
Type *newBasicInt();
Type *newBasicFloat();
Type *newBasicType(int type_index);
Type *newArrayType(Type *elementType, int length);
Type *getElementType(Type *arrayType);

char *typeRepr(Type *type);
char *typeListRepr(TypeNode *typeList);

#endif
