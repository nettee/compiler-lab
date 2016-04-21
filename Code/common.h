#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "type.h"

#define fatal(...) { \
    fprintf(stderr, "\nFatal: "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    assert(0); \
}

#define warn(...) { \
    printf("[warning] %s:%d: ", __FILE__, __LINE__); \
    printf(__VA_ARGS__); \
    printf("\n"); \
}

#define info(...) { \
    printf("[info] %s:%d: ", __FILE__, __LINE__); \
    printf(__VA_ARGS__); \
    printf("\n"); \
}

#define debug(...) { \
    printf("[debug] %s:%d: ", __FILE__, __LINE__); \
    printf(__VA_ARGS__); \
    printf("\n"); \
}

#define RELOP_LT 1091
#define RELOP_LE 1092
#define RELOP_GT 1093
#define RELOP_GE 1094
#define RELOP_EQ 1095
#define RELOP_NE 1096

char *strdup_(char *);

#endif
