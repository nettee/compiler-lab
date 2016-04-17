#ifndef __ST_H__
#define __ST_H__

typedef struct TypeNode_ {
    struct TypeNode_ *next;
    Type *type;
} TypeNode;

void install_variable(char *text, Type *type);
void install_function(char *name, Type *returnType, TypeNode *paramTypeList);
int contains_symbol(char *name);
int contains_variable(char *name);
int contains_function(char *name);

Type *retrieve_variable_type(char *name);

#endif
