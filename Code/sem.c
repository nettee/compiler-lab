#include <stdio.h>
#include <stdlib.h>

#include "syntax.tab.h"
#include "common.h"
#include "pt.h"
#include "st.h"

int nr_semantics_error = 0;

typedef void (*funcptr)(void *);

static void visit(void *node);

static int indent = -1;

#define error(error_id, node, ...) { \
    int lineno = ((int *)node)[1]; \
    printf("Error type %d at Line %d: ", error_id, lineno); \
    printf(__VA_ARGS__); \
    printf("\n"); \
    nr_semantics_error++; \
}

// ===== check variable error =====

static bool check_variable_reference(void *node, char *name) {
    if (!contains_variable(name)) {
        error(1, node, "Undefined variable '%s'", name);
        return false;
    }
    return true;
}

static bool check_variable_definition(void *node, char *name) {
    if (contains_variable(name)) {
        if (in_nested_env()) {
            error(15, node, "Redefined field '%s'", name);
        } else {
            error(3, node, "Redefined variable '%s'", name);
        }
        return false;
    } else if (contains_struct__name(name)) {
        error(3, node, "Variable name '%s' conflicts with type 'struct %s'",
                name, name);
    }
    return true;
}

// ===== check function error =====

static bool check_function_call(void *node, char *name, Args *args) {
    if (contains_variable(name)) {
        error(11, node, "'%s' is not a function", name);
        return false;
    } else if (!contains_function_defined(name)) {
        error(2, node, "Undefined function '%s'", name);
        return false;
    } else {
        // assert: always able to retrieve
        TypeNode *paramTypeList = retrieve_function_paramTypeList(name);
        TypeNode *argTypeList = (args == NULL) ? NULL
                : args->attr_argTypeListTail;
        if (!isEqvTypeList(paramTypeList, argTypeList)) {
            error(9, node, "Function '%s' is not applicable for arguments '%s'",
                    typeListRepr(paramTypeList), typeListRepr(argTypeList));
            return false;
        }
    }
    return true;
}

static bool check_function_definition(void *node, char *name,
        Type *returnType, TypeNode *paramTypeList) {
    if (contains_function_defined(name)) {
        error(4, node, "Redefined function '%s'", name);
        return false;
    } else if (contains_function_declared(name)) {
        Type *returnType0 = retrieve_function_returnType(name);
        TypeNode *paramTypeList0 = retrieve_function_paramTypeList(name);
        if (!isEqvType(returnType, returnType0)
                || !isEqvTypeList(paramTypeList, paramTypeList0)) {
            error(19, node, "Incompatible declaration for function '%s'", name);
            return false;
        }
    }
    return true;
}

static bool check_function_declaration(void *node, char *name,
        Type *returnType, TypeNode *paramTypeList) {
    if (contains_function_declared(name)
            || contains_function_defined(name)) {
        Type *returnType0 = retrieve_function_returnType(name);
        TypeNode *paramTypeList0 = retrieve_function_paramTypeList(name);
        if (!isEqvType(returnType, returnType0)
                || !isEqvTypeList(paramTypeList, paramTypeList0)) {
            error(19, node, "Inconsistent declaration of function '%s'", name);
            return false;
        }
    }
    return true;
}

// ===== check lvalue error =====

static bool check_assignment_lvalue(void *node, bool is_lvalue) {
    if (!is_lvalue) {
        error(6, node, "The left-hand side of assignment must be lvalue");
    }
    return true;
}

// ===== check expression type error =====

static bool check_assignment_type(void *node, Type *t1, Type *t2) {
    if (!isEqvType(t1, t2)) {
        error(5, node, "Cannot assign '%s' to '%s'", typeRepr(t2), typeRepr(t1));
        return false;
    }
    return true;
}

static bool check_infix_type(void *node, Type *t1, Type *t2) {
    if (!isEqvType(t1, t2)) {
        error(7, node, "Type mismatched for infix expression operands");
        return false;
    }
    return true;
}

static bool check_logic_type(void *node, Type *t) {
    if (!isBasicIntType(t)) {
        error(7, node, "Type 'int' expected for logic expression");
        return false;
    }
    return true;
}

static bool check_if_expression_type(void *node, Type *t) {
    if (!isBasicIntType(t)) {
        error(7, node, "Type 'int' expected for if/while expression");
        return false;
    }
    return true;
}

static bool check_function_returnType(void *node,
        Type *returnType, Type *expType) {
    if (!isEqvType(returnType, expType)) {
        error(8, node, "Incompatible return type, expected '%s', actual '%s'", typeRepr(expType), typeRepr(returnType));
        return false;
    }
    return true;
}

static bool check_array_subscript_base_type(void *node, Type *t) {
    if (!isArrayType(t)) {
        error(10, node, "Type '%s' cannot be subscripted", typeRepr(t));
        return false;
    }
}

static bool check_array_subscript_index_type(void *node, Type *t) {
    if (!isBasicIntType(t)) {
        error(12, node, "Type '%s' cannot be array index", typeRepr(t));
        return false;
    }
    return true;
}


// ===== check structure error =====

static bool check_structure_definition(void *node, Type *type) {
    if (contains_struct__type(type)) {
        error(16, node, "Redefined structure '%s'", type->structure.name);
        return false;
    }
    return true;
}

static bool check_structure_reference(void *node, char *name) {
    if (!contains_struct__name(name)) {
        error(17, node, "Undefined structure '%s'", name);
        return false;
    }
    return true;
}

static bool check_structure_field_access(void *node, 
        Type *structType, char *fieldName) {
    if (isArbitType(structType) || !isStructureType(structType)) {
        error(13, node, "Illegal use of '.' on non-struct");
        return false;
    } else if (!hasField(structType, fieldName)) {
        error(14, node, "Structure '%s' has no field '%s'",
                structType->structure.name, fieldName);
        return false;
    }
    return true;
}

static bool check_structure_field_initialize(void *node, 
        bool initialized) {
    if (initialized && in_nested_env()) {
        error(15, node, "Initialized struct field");
        return false;
    }
    return true;
}

static void visitProgram(void *node) {
    Program *program = (Program *)node;
    visit(program->extDefList);
}

static void visitExtDefList(void *node) {
    /* may produce epsilon */
    ExtDefList *extDefList = (ExtDefList *)node;
    if (extDefList->extDef != NULL) {
        visit(extDefList->extDef);
        visit(extDefList->extDefList);
    }
}

static void visitExtDef(void *node) {
    ExtDef *extDef = (ExtDef *)node;

    if (extDef->extdef_kind == EXT_DEF_T_VAR) {
        visit(extDef->var.specifier);
        extDef->var.extDecList->attr_type 
                = extDef->var.specifier->attr_type;
        visit(extDef->var.extDecList);

    } else if (extDef->extdef_kind == EXT_DEF_T_STRUCT) {
        visit(extDef->struct_.specifier);

    } else if (extDef->extdef_kind == EXT_DEF_T_FUN) {
        visit(extDef->fun.specifier);
        extDef->fun.funDec->attr_returnType
                = extDef->fun.specifier->attr_type;
        visit(extDef->fun.funDec);

        check_function_definition(extDef,
                extDef->fun.funDec->id_text,
                extDef->fun.funDec->attr_returnType,
                extDef->fun.funDec->attr_paramTypeList);
        install_function_defined(extDef->fun.funDec->id_text,
            extDef->fun.funDec->attr_returnType,
            extDef->fun.funDec->attr_paramTypeList);

        extDef->fun.compSt->attr_func_returnType
                = extDef->fun.funDec->attr_returnType;
        visit(extDef->fun.compSt);

    } else if (extDef->extdef_kind == EXT_DEF_T_FUN_DEC) {
        visit(extDef->fun.specifier);
        extDef->fun.funDec->attr_returnType
                = extDef->fun.specifier->attr_type;
        visit(extDef->fun.funDec);

        check_function_declaration(extDef,
                extDef->fun.funDec->id_text,
                extDef->fun.funDec->attr_returnType,
                extDef->fun.funDec->attr_paramTypeList);
        install_function_declared(extDef->fun.funDec->id_text,
            extDef->fun.funDec->attr_returnType,
            extDef->fun.funDec->attr_paramTypeList,
            ((int *)node)[1]);

    } else {
        fatal("unknown extdef_type");
    }
}

static void visitExtDecList(void *node) {
    ExtDecList *extDecList = (ExtDecList *)node;
    extDecList->varDec->attr_type = extDecList->attr_type;
    visit(extDecList->varDec);
    if (extDecList->extDecList != NULL) {
        extDecList->extDecList->attr_type 
                = extDecList->attr_type;
        visit(extDecList->extDecList);
    }
}

static void visitSpecifier(void *node) {
    Specifier *specifier = (Specifier *)node;
    if (specifier->specifier_kind == SPECIFIER_T_BASIC) {
        specifier->attr_type = newBasicType(specifier->type_index);
    } else if (specifier->specifier_kind ==  SPECIFIER_T_STRUCT) {
        visit(specifier->structSpecifier);
        specifier->attr_type = specifier->structSpecifier->attr_type;
    } else {
        fatal("unknown specifier_type");
    }
}

static void visitStructSpecifier(void *node) {
    StructSpecifier *structSpecifier = (StructSpecifier *)node;
    int kind = structSpecifier->structspecifier_kind;
    if (kind == STRUCT_SPECIFIER_T_DEC) {
        visit(structSpecifier->dec.tag);
        char *name = structSpecifier->dec.tag->attr_name;
        bool exist = check_structure_reference(structSpecifier, name);
        structSpecifier->attr_type = exist
                ? retrieve_struct(name)
                : getArbitType();

    } else if (kind == STRUCT_SPECIFIER_T_DEF) {
        visit(structSpecifier->def.optTag);
        enter_new_env();
        visit(structSpecifier->def.defList);
        FieldNode *fieldList = exit_current_env();
        Type *structType = newStructureType(
                structSpecifier->def.optTag->attr_name,
                fieldList);
        check_structure_definition(structSpecifier, structType);
        install_struct(structType);
        structSpecifier->attr_type = structType;

    } else {
        fatal("unknown structspecifier_type");
    }
}

static void visitOptTag(void *node) {
    /* may produce epsilon */
    OptTag *optTag = (OptTag *)node;
    optTag->attr_name = optTag->id_text;
}

static void visitTag(void *node) {
    Tag *tag = (Tag *)node;
    tag->attr_name = tag->id_text;
}

static void visitVarDec(void *node) {
    VarDec *varDec = (VarDec *)node;
    if (varDec->vardec_kind == VAR_DEC_T_ID) {
        check_variable_definition(varDec, varDec->id_text);
        install_variable(varDec->id_text, varDec->attr_type);

    } else if (varDec->vardec_kind == VAR_DEC_T_DIM) {
        varDec->dim.varDec->attr_type = newArrayType(
                varDec->attr_type, varDec->dim.int_value);
        visit(varDec->dim.varDec);
        // new feature: integrate back
        varDec->attr_type = varDec->dim.varDec->attr_type;

    } else {
        fatal("unknown vardec_type");
    }
}

static void visitFunDec(void *node) {
    FunDec *funDec = (FunDec *)node;
    if (funDec->varList != NULL) {
        visit(funDec->varList);
    }
    funDec->attr_paramTypeList = (funDec->varList == NULL) 
            ? NULL : funDec->varList->attr_paramTypeListTail;

}

static void visitVarList(void *node) {
    VarList *varList = (VarList *)node;
    visit(varList->paramDec);
    TypeNode *paramTypeListTail = NULL;
    if (varList->varList != NULL) {
        visit(varList->varList);
        paramTypeListTail = varList->varList->attr_paramTypeListTail;
    }
    TypeNode *typeNode = malloc(sizeof(TypeNode));
    typeNode->next = paramTypeListTail;
    typeNode->type = varList->paramDec->attr_paramType;
    debug("paramType: %s", typeRepr(varList->paramDec->attr_paramType));
    varList->attr_paramTypeListTail = typeNode;
}

static void visitParamDec(void *node) {
    ParamDec *paramDec = (ParamDec *)node;
    visit(paramDec->specifier);
    paramDec->varDec->attr_type = paramDec->specifier->attr_type;
    visit(paramDec->varDec);
    debug("paramDec->varDec->attr_type = %s", typeRepr(paramDec->varDec->attr_type));
    // new feature: array type function parameter
    paramDec->attr_paramType = paramDec->varDec->attr_type;
}

static void visitCompSt(void *node) {
    CompSt *compSt = (CompSt *)node;
    visit(compSt->defList);
    compSt->stmtList->attr_func_returnType = compSt->attr_func_returnType;
    visit(compSt->stmtList);
}

static void visitStmtList(void *node) {
    /* may produce epsilon */
    StmtList *stmtList = (StmtList *)node;
    if (stmtList->stmt != NULL) {
        // stmtList->stmt and stmtList->stmtList
        // will both be non-null
        stmtList->stmt->attr_func_returnType 
                = stmtList->attr_func_returnType;
        visit(stmtList->stmt);
        stmtList->stmtList->attr_func_returnType 
                = stmtList->attr_func_returnType;
        visit(stmtList->stmtList);
    }
}

static void visitStmt(void *node) {
    Stmt *stmt = (Stmt *)node;
    if (stmt->stmt_kind == STMT_T_EXP) {
        visit(stmt->exp.exp);

    } else if (stmt->stmt_kind == STMT_T_COMP_ST) {
        stmt->compst.compSt->attr_func_returnType
                = stmt->attr_func_returnType;
        visit(stmt->compst.compSt);

    } else if (stmt->stmt_kind == STMT_T_RETURN) {
        visit(stmt->return_.exp);
        check_function_returnType(stmt,
                stmt->attr_func_returnType,
                stmt->return_.exp->attr_type);

    } else if (stmt->stmt_kind == STMT_T_IF) {
        visit(stmt->if_.exp);
        check_if_expression_type(stmt, stmt->if_.exp->attr_type);
        stmt->if_.then_stmt->attr_func_returnType 
                = stmt->attr_func_returnType;
        visit(stmt->if_.then_stmt);

    } else if (stmt->stmt_kind == STMT_T_IF_ELSE) {
        visit(stmt->ifelse.exp);
        check_if_expression_type(stmt, stmt->ifelse.exp->attr_type);
        stmt->ifelse.then_stmt->attr_func_returnType 
                = stmt->attr_func_returnType;
        visit(stmt->ifelse.then_stmt);
        stmt->ifelse.else_stmt->attr_func_returnType 
                = stmt->attr_func_returnType;
        visit(stmt->ifelse.else_stmt);

    } else if (stmt->stmt_kind == STMT_T_WHILE) {
        visit(stmt->while_.exp);
        check_if_expression_type(stmt, stmt->while_.exp->attr_type);
        stmt->while_.stmt->attr_func_returnType
                = stmt->attr_func_returnType;
        visit(stmt->while_.stmt);

    } else {
        fatal("unknown stmt_type");
    }
}

static void visitDefList(void *node) {
    /* may produce epsilon */
    DefList *defList = (DefList *)node;
    if (defList->def != NULL) {
        visit(defList->def);
        visit(defList->defList);
    }
}

static void visitDef(void *node) {
    Def *def = (Def *)node;
    visit(def->specifier);
    def->decList->attr_type = def->specifier->attr_type;
    visit(def->decList);
}

static void visitDecList(void *node) {
    DecList *decList = (DecList *)node;
    decList->dec->attr_type = decList->attr_type;
    visit(decList->dec);
    if (decList->decList != NULL) {
        decList->decList->attr_type = decList->attr_type;
        visit(decList->decList);
    }
}

static void visitDec(void *node) {
    Dec *dec = (Dec *)node;
    dec->varDec->attr_type = dec->attr_type;
    visit(dec->varDec);
    check_structure_field_initialize(dec, dec->exp != NULL);
    if (dec->exp != NULL) {
        visit(dec->exp);
        check_assignment_type(dec,
                dec->varDec->attr_type,
                dec->exp->attr_type);
    }

}

static void visitExp(void *node) {
    Exp *exp = (Exp *)node;

    if (exp->exp_kind == EXP_T_INFIX) {

        visit(exp->infix.exp_left);
        visit(exp->infix.exp_right);

        if (exp->infix.op == PLUS
                || exp->infix.op == MINUS
                || exp->infix.op == STAR
                || exp->infix.op == DIV) {
            bool compatible = check_infix_type(exp,
                    exp->infix.exp_left->attr_type,
                    exp->infix.exp_right->attr_type);
            exp->attr_type = compatible 
                    ? exp->infix.exp_left->attr_type
                    : getArbitType();

        } else if (exp->infix.op == RELOP) {
            check_infix_type(exp,
                    exp->infix.exp_left->attr_type,
                    exp->infix.exp_right->attr_type);
            exp->attr_type = getBasicInt();

        } else if (exp->infix.op == AND
                || exp->infix.op == OR) {
            check_logic_type(exp, exp->infix.exp_left->attr_type);
            check_logic_type(exp, exp->infix.exp_right->attr_type);
            exp->attr_type = getBasicInt();

        } else if (exp->infix.op == ASSIGNOP) {
            check_assignment_type(exp,
                    exp->infix.exp_left->attr_type,
                    exp->infix.exp_right->attr_type);
            check_assignment_lvalue(exp,
                    exp->infix.exp_left->attr_lvalue);
            exp->attr_type = exp->infix.exp_left->attr_type;

        } else {
            fatal("unknown exp->infix.op");
        }
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_PAREN) {
        visit(exp->paren.exp);
        exp->attr_type = exp->paren.exp->attr_type;
        exp->attr_lvalue = exp->paren.exp->attr_lvalue;

    } else if (exp->exp_kind == EXP_T_UNARY) {
        visit(exp->unary.exp);

        if (exp->unary.op == MINUS) {
            exp->attr_type = exp->unary.exp->attr_type;
        } else if (exp->unary.op == NOT) {
            check_logic_type(exp,
                    exp->unary.exp->attr_type);
            exp->attr_type = getBasicInt();
        } else {
            fatal("unknown exp->unary.op");
        }
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_CALL) {
        if (exp->call.args != NULL) {
            visit(exp->call.args);
        }
        bool callable = check_function_call(exp, exp->call.id_text,
                exp->call.args); 
        exp->attr_type = callable
                ? retrieve_function_returnType(exp->call.id_text)
                : getArbitType();
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_SUBSCRIPT) {
        visit(exp->subscript.array);
        visit(exp->subscript.index);
        Type *baseType = exp->subscript.array->attr_type;
        Type *indexType = exp->subscript.index->attr_type;
        bool subscriptable = check_array_subscript_base_type(exp, baseType);
        check_array_subscript_index_type(exp, indexType);
        exp->attr_type = subscriptable
                ? getElementType(baseType)
                : getArbitType();
        exp->attr_lvalue = true;

    } else if (exp->exp_kind == EXP_T_DOT) {
        visit(exp->dot.exp);
        Type *structType = exp->dot.exp->attr_type;
        char *fieldName = exp->dot.id_text;
        bool legal = check_structure_field_access(exp, structType, fieldName);
        exp->attr_type = legal
                ? getFieldType(structType, fieldName)
                : getArbitType();
        exp->attr_lvalue = true;

    } else if (exp->exp_kind == EXP_T_ID) {
        bool exist = check_variable_reference(exp, exp->id_text);
        exp->attr_type = exist 
                ? retrieve_variable_type(exp->id_text)
                : getArbitType();
        exp->attr_lvalue = true;

    } else if (exp->exp_kind == EXP_T_INT) {
        exp->attr_type = getBasicInt();
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_FLOAT) {
        exp->attr_type = getBasicFloat();
        exp->attr_lvalue = false;

    } else {
        fatal("unknown exp_type");
    }
}

static void visitArgs(void *node) {
    Args *args = (Args *)node;
    visit(args->exp);
    TypeNode *argTypeListTail = NULL;
    if (args->args != NULL) {
        visit(args->args);
        argTypeListTail = args->args->attr_argTypeListTail;
    }
    TypeNode *typeNode = malloc(sizeof(TypeNode));
    typeNode->next = argTypeListTail;
    typeNode->type = args->exp->attr_type;
    args->attr_argTypeListTail = typeNode;
}

static funcptr visitor_table[] = {
    visitProgram,
    visitExtDefList,
    visitExtDef,
    visitExtDecList,
    visitSpecifier,
    visitStructSpecifier,
    visitOptTag,
    visitTag,
    visitVarDec,
    visitFunDec,
    visitVarList,
    visitParamDec,
    visitCompSt,
    visitStmtList,
    visitStmt,
    visitDefList,
    visitDef,
    visitDecList,
    visitDec,
    visitExp,
    visitArgs,
};

static void visit(void *node) {
    int type = *(int *)node;
    indent++;
    visitor_table[type-400](node);
    indent--;
}

void semantics_analysis() {
    visit(root);
    check_function_declared_undefined();
}
