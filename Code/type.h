#ifndef __TYPE_H__
#define __TYPE_H__

typedef struct Type_ {
    enum { BASIC, ARRAY, STRUCTURE } kind;
    union {
        int basic;
        struct {
            struct Type_ *element;
            int length;
        } array;
        int structure; // TODO
    };
} Type;

#endif
