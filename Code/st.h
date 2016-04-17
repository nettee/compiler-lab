#ifndef __ST_H__
#define __ST_H__

void install_variable(char *text, Type *type);
int contains_symbol(char *name);
int contains_variable(char *name);
int contains_function(char *name);

Type *retrieve_variable_type(char *name);

#endif
