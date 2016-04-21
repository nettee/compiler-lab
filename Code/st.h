#ifndef __ST_H__
#define __ST_H__

#include "common.h"

void install_variable(char *text, Type *type);
void install_function(char *name, Type *returnType, TypeNode *paramTypeList);
int contains_symbol(char *name);
int contains_variable(char *name);
int contains_function(char *name);

Type *retrieve_variable_type(char *name);
Type *retrieve_function_returnType(char *name);
TypeNode *retrieve_function_paramTypeList(char *name);

void enter_new_env();
FieldNode *exit_current_env();

#endif
