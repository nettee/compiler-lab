#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ST_ID 1901
#define ST_INT 1902
#define ST_FLOAT 1903

typedef struct {
    int type;
    union {
        char *id_str;
        int int_value;
        float float_value;
    };
} Symbol;
Symbol symbols[40];

static int nr_symbol = 0;

static char *strdup_(char *src) {
    int N = strlen(src);
    char *dest = malloc(N+1);
    strcpy(dest, src);
    return dest;
}

/* 
 * all these install function returns index of symbol
 * in the symbol table
 */

int install_id(char *str) {
    /* find if this id is already in symbol table
     * if so, return index of the id
     */
    int i;
    for (i = 0; i < nr_symbol; i++) {
        if (symbols[i].type == ST_ID) {
            if (strcmp(symbols[i].id_str, str) == 0) {
                return i;
            }
        }
    }

    /* create new symbol table item */
    Symbol *symbol = &symbols[nr_symbol];
    symbol->type = ST_ID;
    symbol->id_str = strdup_(str);

    int index = nr_symbol;
    nr_symbol++;
    return index;
}

int install_int(int num) {

}

int install_float(float num) {

}

char *resolve_id(int index) {
    if (symbols[index].type != ST_ID) {
        printf("fatal: symbol is not id\n");
    }
    return symbols[index].id_str;
}


/* for test */
void print_symbol_table() {
    printf("Symbol table:\n");
    int i;
    for (i = 0; i < nr_symbol; i++) {
        printf("[%02d] ", i);
        Symbol *s = &symbols[i];
        switch (s->type) {
        case ST_ID:
            printf("(id) %s\n", s->id_str);
            break;
        case ST_INT:
            printf("(int) %d\n", s->int_value);
            break;
        case ST_FLOAT:
            printf("(float) %f\n", s->float_value);
            break;
        default:
            printf("fatal: unknown symbol type\n");
        }
    }
    printf("===============\n");
}
