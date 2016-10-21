#include <stdlib.h>
#include <string.h>

#include "common.h"

char *strdup_(char *src) {
    int N = strlen(src);
    char *dest = malloc(N+1);
    strcpy(dest, src);
    return dest;
}
