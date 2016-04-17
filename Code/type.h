#ifndef __TYPE_H__
#define __TYPE_H__

#include "common.h"

typedef int bool;
#define true 1
#define false 0

enum { T_INT, T_FLOAT };

typedef struct Type_ {
    enum { BASIC, ARRAY, STRUCTURE } kind;
    union {
        int basic;
        struct {
            struct Type_ *elementType;
            int length;
        } array;
        int structure; // TODO
    };
} Type;

bool isBasicIntType(Type *t);
bool isBasicType(Type *t);
bool isEqvType(Type *t1, Type *t2);

Type *newBasicInt();
Type *newBasicFloat();
Type *newBasicType(int type_index);
Type *newArrayType(Type *elementType, int length);

void printType(Type *type);

#endif
