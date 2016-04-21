#include <stdio.h>
#include <stdlib.h>

#include "syntax.tab.h"
#include "common.h"
#include "pt.h"
#include "st.h"

int nr_semantics_error = 0;

typedef void (*funcptr)(void *);

static void visit(void *node);

char *terminal_str(int token_enum);
char *nonterminal_str(int nonterminal_enum);

static int indent = -1;

#define print(...) { \
}

#define error(error_id, node, ...) { \
    int lineno = ((int *)node)[1]; \
    printf("Error type %d at Line %d: ", error_id, lineno); \
    printf(__VA_ARGS__); \
    printf("\n"); \
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
    }
    return true;
}

// ===== check function error =====

static bool check_function_call(void *node, char *name, Args *args) {
    if (contains_variable(name)) {
        error(11, node, "'%s' is not a function", name);
        return false;
    } else if (!contains_function(name)) {
        error(2, node, "Undefined function '%s'", name);
        return false;
    } else {
        TypeNode *paramTypeList = retrieve_function_paramTypeList(name);
        TypeNode *argTypeList = args->attr_argTypeListTail;
        if (!isEqvTypeList(paramTypeList, argTypeList)) {
            error(9, node, "Function '%s' is not applicable for arguments '%s'",
                    typeListRepr(paramTypeList), typeListRepr(argTypeList));
            return false;
        }
    }
    return true;
}

static bool check_function_definition(void *node, char *name) {
    if (contains_function(name)) {
        error(4, node, "Redefined function '%s'", name);
        return false;
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
        error(5, node, "Type mismatched for assignment: cannot assign '%s' to '%s'", typeRepr(t2), typeRepr(t1));
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

static bool check_function_returnType(void *node,
        Type *returnType, Type *expType) {
    if (!isEqvType(returnType, expType)) {
        error(8, node, "Incompatible return type, expected '%s', actual '%s'", typeRepr(expType), typeRepr(returnType));
        return false;
    }
    return true;
}

static bool check_array_subscript_type(void *node, Type *baseType, Type *indexType) {
    if (!isArrayType(baseType)) {
        error(10, node, "Type '%s' cannot be subscripted", typeRepr(baseType));
        return false;
    }
    if (!isBasicIntType(indexType)) {
        error(12, node, "Type 'int' expected for array index");
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
    if (!isStructureType(structType)) {
        error(13, node, "Illegal use of '.' on non-struct");
        return false;
    } else if (!hasField(structType, fieldName)) {
        error(14, node, "Structure '%s' has no field '%s'",
                structType->structure.name, fieldName);
        return false;
    }
    return true;
}

static void print_id(char *id_text) {
    indent++;
    print("ID: %s", id_text);
    indent--;
}

static void print_int(int int_value) {
    indent++;
    print("INT: %d", int_value);
    indent--;
}

static void print_float(float float_value) {
    indent++;
    print("FLOAT: %f", float_value);
    indent--;
}

static void print_type(int type_index) {
    indent++;
    switch (type_index) {
    case T_INT:
        print("TYPE: int");
        break;
    case T_FLOAT:
        print("TYPE: float");
        break;
    default:
        printf("Fatal: Unknown specifier->type_value\n");
    }
    indent--;
}

static void print_terminal(int terminal) {
    indent++;
    print("%s", terminal_str(terminal));
    indent--;
}

static void print_relop(int yylval) {
    indent++;
    switch (yylval) {
    case RELOP_LT:
        print("RELOP: <");
        break;
    case RELOP_LE:
        print("RELOP: <=");
        break;
    case RELOP_GT:
        print("RELOP: >");
        break;
    case RELOP_GE:
        print("RELOP: >=");
        break;
    case RELOP_EQ:
        print("RELOP: ==");
        break;
    case RELOP_NE:
        print("RELOP: !=");
        break;
    default:
        printf("fatal: unknown relop yylval\n");
    }
}

static void print_this(void *node) {
    int type = *(int *)node;
    int lineno = ((int *)node)[1];
    print("%s (%d)", nonterminal_str(type), lineno);
}

static void visitProgram(void *node) {
    print_this(node);
    Program *program = (Program *)node;
    visit(program->extDefList);
}

static void visitExtDefList(void *node) {
    /* may produce epsilon */
    ExtDefList *extDefList = (ExtDefList *)node;
    if (extDefList->extDef != NULL) {
        print_this(node);
        visit(extDefList->extDef);
        visit(extDefList->extDefList);
    }
}

static void visitExtDef(void *node) {
    print_this(node);
    ExtDef *extDef = (ExtDef *)node;

    if (extDef->extdef_kind == EXT_DEF_T_VAR) {
        visit(extDef->var.specifier);
        extDef->var.extDecList->attr_type 
                = extDef->var.specifier->attr_type;
        visit(extDef->var.extDecList);

    } else if (extDef->extdef_kind == EXT_DEF_T_STRUCT) {
        visit(extDef->struct_.specifier);
        Type *t = extDef->struct_.specifier->attr_type;
        if (isStructureType(t)) {
            check_structure_definition(extDef, t);
            install_struct(t);
        }

    } else if (extDef->extdef_kind == EXT_DEF_T_FUN) {
        visit(extDef->fun.specifier);
        extDef->fun.funDec->attr_returnType
                = extDef->fun.specifier->attr_type;
        visit(extDef->fun.funDec);
        extDef->fun.compSt->attr_func_returnType
                = extDef->fun.funDec->attr_returnType;
        visit(extDef->fun.compSt);
    } else {
        fatal("unknown extdef_type");
    }
}

static void visitExtDecList(void *node) {
    print_this(node);
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
    print_this(node);
    Specifier *specifier = (Specifier *)node;
    switch (specifier->specifier_kind) {
    case SPECIFIER_T_BASIC:
        specifier->attr_type = newBasicType(specifier->type_index);
        break;
    case SPECIFIER_T_STRUCT:
        visit(specifier->structSpecifier);
        specifier->attr_type = specifier->structSpecifier->attr_type;
        break;
    default:
        printf("fatal: unknown specifier_type\n");
    }
}

static void visitStructSpecifier(void *node) {
    print_this(node);
    StructSpecifier *structSpecifier = (StructSpecifier *)node;
    int kind = structSpecifier->structspecifier_kind;
    if (kind == STRUCT_SPECIFIER_T_DEC) {
        visit(structSpecifier->dec.tag);
        char *name = structSpecifier->dec.tag->attr_name;
        check_structure_reference(structSpecifier, name);
        structSpecifier->attr_type = retrieve_struct(name);
    } else if (kind == STRUCT_SPECIFIER_T_DEF) {
        visit(structSpecifier->def.optTag);
        enter_new_env();
        visit(structSpecifier->def.defList);
        FieldNode *fieldList = exit_current_env();
        structSpecifier->attr_type = newStructureType(
                structSpecifier->def.optTag->id_text,
                fieldList);
    } else {
        fatal("unknown structspecifier_type");
    }
}

static void visitOptTag(void *node) {
    /* may produce epsilon */
    OptTag *optTag = (OptTag *)node;
    if (optTag->id_text != NULL) {
        print_this(node);
        print_id(optTag->id_text);
    }
}

static void visitTag(void *node) {
    print_this(node);
    Tag *tag = (Tag *)node;
    tag->attr_name = tag->id_text;
}

static void visitVarDec(void *node) {
    print_this(node);
    VarDec *varDec = (VarDec *)node;
    if (varDec->vardec_kind == VAR_DEC_T_ID) {
        check_variable_definition(varDec, varDec->id_text);
        install_variable(varDec->id_text, varDec->attr_type);
    } else if (varDec->vardec_kind == VAR_DEC_T_DIM) {
        varDec->dim.varDec->attr_type = newArrayType(
                varDec->attr_type, varDec->dim.int_value);
        visit(varDec->dim.varDec);
    } else {
        fatal("unknown vardec_type");
    }
}

static void visitFunDec(void *node) {
    print_this(node);
    FunDec *funDec = (FunDec *)node;
    if (funDec->varList != NULL) {
        visit(funDec->varList);
    }
    check_function_definition(funDec, funDec->id_text);

    install_function(funDec->id_text,
            funDec->attr_returnType,
            (funDec->varList == NULL) ? NULL : funDec->varList->attr_paramTypeListTail);
}

static void visitVarList(void *node) {
    print_this(node);
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
    varList->attr_paramTypeListTail = typeNode;
}

static void visitParamDec(void *node) {
    print_this(node);
    ParamDec *paramDec = (ParamDec *)node;
    visit(paramDec->specifier);
    paramDec->varDec->attr_type = paramDec->specifier->attr_type;
    visit(paramDec->varDec);
    paramDec->attr_paramType = paramDec->specifier->attr_type;
}

static void visitCompSt(void *node) {
    print_this(node);
    CompSt *compSt = (CompSt *)node;
    visit(compSt->defList);
    compSt->stmtList->attr_func_returnType = compSt->attr_func_returnType;
    visit(compSt->stmtList);
}

static void visitStmtList(void *node) {
    /* may produce epsilon */
    StmtList *stmtList = (StmtList *)node;
    if (stmtList->stmt != NULL) {
        stmtList->stmt->attr_func_returnType 
                = stmtList->attr_func_returnType;
        visit(stmtList->stmt);
        stmtList->stmtList->attr_func_returnType 
                = stmtList->attr_func_returnType;
        visit(stmtList->stmtList);
    }
}

static void visitStmt(void *node) {
    print_this(node);
    Stmt *stmt = (Stmt *)node;
    if (stmt->stmt_kind == STMT_T_EXP) {
        visit(stmt->exp.exp);
        print_terminal(SEMI);

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
        stmt->if_.then_stmt->attr_func_returnType 
                = stmt->attr_func_returnType;
        visit(stmt->if_.then_stmt);

    } else if (stmt->stmt_kind == STMT_T_IF_ELSE) {
        visit(stmt->ifelse.exp);
        stmt->ifelse.then_stmt->attr_func_returnType 
                = stmt->attr_func_returnType;
        visit(stmt->ifelse.then_stmt);
        stmt->ifelse.else_stmt->attr_func_returnType 
                = stmt->attr_func_returnType;
        visit(stmt->ifelse.else_stmt);
    } else if (stmt->stmt_kind == STMT_T_WHILE) {
        visit(stmt->while_.exp);
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
        print_this(node);
        visit(defList->def);
        visit(defList->defList);
    }
}

static void visitDef(void *node) {
    print_this(node);
    Def *def = (Def *)node;
    visit(def->specifier);
    def->decList->attr_type = def->specifier->attr_type;
    visit(def->decList);
}

static void visitDecList(void *node) {
    print_this(node);
    DecList *decList = (DecList *)node;
    decList->dec->attr_type = decList->attr_type;
    visit(decList->dec);
    if (decList->decList != NULL) {
        decList->decList->attr_type = decList->attr_type;
        visit(decList->decList);
    }
}

static void visitDec(void *node) {
    print_this(node);
    Dec *dec = (Dec *)node;
    dec->varDec->attr_type = dec->attr_type;
    visit(dec->varDec);
    if (dec->exp != NULL) {
        visit(dec->exp);
        check_assignment_type(dec,
                dec->varDec->attr_type,
                dec->exp->attr_type);
    }
}

static void visitExp(void *node) {
    print_this(node);
    Exp *exp = (Exp *)node;

    if (exp->exp_kind == EXP_T_INFIX) {

        visit(exp->infix.exp_left);
        if (exp->infix.op == RELOP) {
            print_relop(exp->infix.op_yylval);
        } else {
            print_terminal(exp->infix.op);
        }
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
            exp->attr_type = exp->infix.exp_left->attr_type;

        } else if (exp->infix.op == AND
                || exp->infix.op == OR) {
            check_logic_type(exp,
                    exp->infix.exp_left->attr_type);
            check_logic_type(exp,
                    exp->infix.exp_right->attr_type);
            exp->attr_type = newBasicInt();

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
            exp->attr_type = newBasicInt();
        } else {
            fatal("unknown exp->unary.op");
        }
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_CALL) {
        if (exp->call.args != NULL) {
            visit(exp->call.args);
        }
        check_function_call(exp, exp->call.id_text,
                exp->call.args); 
        exp->attr_type = retrieve_function_returnType(exp->call.id_text);
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_SUBSCRIPT) {
        visit(exp->subscript.array);
        visit(exp->subscript.index);
        Type *baseType = exp->subscript.array->attr_type;
        Type *indexType = exp->subscript.index->attr_type;
        check_array_subscript_type(exp, baseType, indexType);
        exp->attr_type = isArrayType(baseType)
            ? getElementType(baseType)
            : newArbitType();
        exp->attr_lvalue = true;

    } else if (exp->exp_kind == EXP_T_DOT) {
        visit(exp->dot.exp);
        Type *structType = exp->dot.exp->attr_type;
        char *fieldName = exp->dot.id_text;
        check_structure_field_access(exp, structType, fieldName);
        exp->attr_type = getFieldType(structType, fieldName);
        exp->attr_lvalue = true;

    } else if (exp->exp_kind == EXP_T_ID) {
        check_variable_reference(exp, exp->id_text);
        exp->attr_type = retrieve_variable_type(exp->id_text);
        exp->attr_lvalue = true;

    } else if (exp->exp_kind == EXP_T_INT) {
        exp->attr_type = newBasicInt();
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_FLOAT) {
        exp->attr_type = newBasicFloat();
        exp->attr_lvalue = false;

    } else {
        fatal("unknown exp_type");
    }
}

static void visitArgs(void *node) {
    print_this(node);
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
    //printf("node = %p\n", node);
    int type = *(int *)node;
    //printf("type = %d\n", type);
    indent++;
    visitor_table[type-400](node);
    indent--;
}

void semantics_analysis() {
    visit(root);
}
