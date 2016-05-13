#ifndef __ST_H__
#define __ST_H__

#include "common.h"

void install_struct(Type *type);
bool contains_struct__type(Type *type);
bool contains_struct__name(char *name);
Type *retrieve_struct(char *name);

void install_variable(char *text, Type *type);
void install_function_defined(char *name, Type *returnType, TypeNode *paramTypeList);
void install_function_declared(char *name, Type *returnType, TypeNode *paramTypeList, int lineno);
int contains_symbol(char *name);
int contains_variable(char *name);
int contains_function_defined(char *name);
int contains_function_declared(char *name);

Type *retrieve_variable_type(char *name);
int retrieve_variable_rank(char *name);
Type *retrieve_function_returnType(char *name);
TypeNode *retrieve_function_paramTypeList(char *name);

bool check_function_declared_undefined();

void enter_new_env();
FieldNode *exit_current_env();
bool in_nested_env();

#endif
