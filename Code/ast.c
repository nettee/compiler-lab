#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

void *root;

Program *newProgram(void *extDefList) {
    Program *program = malloc(sizeof(Program));
    program->type = PROGRAM;
    program->parent = NULL;
    program->extDefList = extDefList;
    return program;
}

ExtDefList *newExtDefList() {
    ExtDefList *extDefList = malloc(sizeof(ExtDefList));
    extDefList->type = EXT_DEF_LIST;
    extDefList->parent = NULL;
    extDefList->list = NULL;
    return extDefList;
}

ExtDefList *ExtDefList_add(void *arg0, void *arg1) {
    ExtDef *extDef = (ExtDef *)arg0;
    ExtDefList *extDefList = (ExtDefList *)arg1;

    /* insert extDef before head of extDefList->list */
    // TODO
}




VarDec *newVarDec_1(int id_value) {
    VarDec *varDec = malloc(sizeof(VarDec));
    varDec->type = VAR_DEC;
    varDec->parent = NULL;
    varDec->id_index = id_value;
    return varDec;
}

Dec *newDec_1(void *arg0) {
    VarDec *varDec = (VarDec *)arg0;
    
    Dec *dec = malloc(sizeof(Dec));
    dec->type = DEC;
    dec->parent = NULL;
    dec->varDec = varDec;

    return dec;
}
