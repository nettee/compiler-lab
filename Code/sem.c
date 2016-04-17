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

static void check_undefine_variable(void *node, char *name) {
    if (!contains_variable(name)) {
        error(1, node, "Undefined variable '%s'", name);
    }
}

static void check_redefine_variable(void *node, char *name) {
    if (contains_variable(name)) {
        error(3, node, "Redefined variable '%s'", name);
    }
}

static void check_assignment_type(void *node, Type *t1, Type *t2) {
    if (!isEqvType(t1, t2)) {
        error(5, node, "Type mismatched for assignment");
    }
}

static void check_assignment_lvalue(void *node, bool is_lvalue) {
    if (!is_lvalue) {
        error(6, node, "The left-hand side of assignment must be lvalue");
    }
}

static void check_infix_type(void *node, Type *t1, Type *t2) {
    if (!isEqvType(t1, t2)) {
        error(7, node, "Type mismatched for infix expression operands");
    }
}

static void check_logic_type(void *node, Type *t) {
    if (!isBasicIntType(t)) {
        error(7, node, "Type 'int' expected for logic expression");
    }
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
    switch (extDef->extdef_kind) {
    case EXT_DEF_T_VAR:
        visit(extDef->var.specifier);
        visit(extDef->var.extDecList);
        print_terminal(SEMI);
        break;
    case EXT_DEF_T_STRUCT:
        visit(extDef->struct_.specifier);
        print_terminal(SEMI);
        break;
    case EXT_DEF_T_FUN:
        visit(extDef->fun.specifier);
        visit(extDef->fun.funDec);
        visit(extDef->fun.compSt);
        break;
    default:
        printf("fatal: unknown extdef_type\n");
    }
}

static void visitExtDecList(void *node) {
    print_this(node);
    ExtDecList *extDecList = (ExtDecList *)node;
    visit(extDecList->varDec);
    if (extDecList->extDecList != NULL) {
        print_terminal(COMMA);
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
        break;
    default:
        printf("fatal: unknown specifier_type\n");
    }
}

static void visitStructSpecifier(void *node) {
    print_this(node);
    StructSpecifier *structSpecifier = (StructSpecifier *)node;
    switch (structSpecifier->structspecifier_kind) {
    case STRUCT_SPECIFIER_T_DEC:
        print_terminal(STRUCT);
        visit(structSpecifier->dec.tag);
        break;
    case STRUCT_SPECIFIER_T_DEF:
        print_terminal(STRUCT);
        visit(structSpecifier->def.optTag);
        print_terminal(LC);
        visit(structSpecifier->def.defList);
        print_terminal(RC);
        break;
    default:
        printf("fatal: unknown structspecifier_type\n");
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
    print_id(tag->id_text);
}

static void visitVarDec(void *node) {
    print_this(node);
    VarDec *varDec = (VarDec *)node;
    switch (varDec->vardec_kind) {
    case VAR_DEC_T_ID:
        check_redefine_variable(varDec, varDec->id_text);
        install_variable(varDec->id_text, varDec->attr_type);
        break;
    case VAR_DEC_T_DIM:
        visit(varDec->dim.varDec);
        print_terminal(LB);
        print_int(varDec->dim.int_value);
        print_terminal(RB);
        break;
    default:
        printf("fatal: unknown vardec_type\n");
    }
}

static void visitFunDec(void *node) {
    print_this(node);
    FunDec *funDec = (FunDec *)node;
    print_id(funDec->id_text);
    print_terminal(LP);
    if (funDec->varList != NULL) {
        visit(funDec->varList);
    }
    print_terminal(RP);
}

static void visitVarList(void *node) {
    print_this(node);
    VarList *varList = (VarList *)node;
    visit(varList->paramDec);
    if (varList->varList != NULL) {
        print_terminal(COMMA);
        visit(varList->varList);
    }
}

static void visitParamDec(void *node) {
    print_this(node);
    ParamDec *paramDec = (ParamDec *)node;
    visit(paramDec->specifier);
    visit(paramDec->varDec);
}

static void visitCompSt(void *node) {
    print_this(node);
    CompSt *compSt = (CompSt *)node;
    print_terminal(LC);
    visit(compSt->defList);
    visit(compSt->stmtList);
    print_terminal(RC);
}

static void visitStmtList(void *node) {
    /* may produce epsilon */
    StmtList *stmtList = (StmtList *)node;
    if (stmtList->stmt != NULL) {
        print_this(node);
        visit(stmtList->stmt);
        visit(stmtList->stmtList);
    }
}

static void visitStmt(void *node) {
    print_this(node);
    Stmt *stmt = (Stmt *)node;
    switch (stmt->stmt_kind) {
    case STMT_T_EXP:
        visit(stmt->exp.exp);
        print_terminal(SEMI);
        break;
    case STMT_T_COMP_ST:
        visit(stmt->compst.compSt);
        break;
    case STMT_T_RETURN:
        print_terminal(RETURN);
        visit(stmt->return_.exp);
        print_terminal(SEMI);
        break;
    case STMT_T_IF:
        print_terminal(IF);
        print_terminal(LP);
        visit(stmt->if_.exp);
        print_terminal(RP);
        visit(stmt->if_.then_stmt);
        break;
    case STMT_T_IF_ELSE:
        print_terminal(IF);
        print_terminal(LP);
        visit(stmt->ifelse.exp);
        print_terminal(RP);
        visit(stmt->ifelse.then_stmt);
        print_terminal(ELSE);
        visit(stmt->ifelse.else_stmt);
        break;
    case STMT_T_WHILE:
        print_terminal(WHILE);
        print_terminal(LP);
        visit(stmt->while_.exp);
        print_terminal(RP);
        visit(stmt->while_.stmt);
        break;
    default:
        printf("fatal: unknown stmt_type\n");
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
    print_terminal(SEMI);
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
            check_infix_type(exp,
                    exp->infix.exp_left->attr_type,
                    exp->infix.exp_right->attr_type);
            exp->attr_type = exp->infix.exp_left->attr_type;

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
        print_terminal(LP);
        visit(exp->paren.exp);
        print_terminal(RP);
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_UNARY) {
        print_terminal(exp->unary.op);
        visit(exp->unary.exp);
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_CALL) {
        print_id(exp->call.id_text);
        print_terminal(LP);
        if (exp->call.args != NULL) {
            visit(exp->call.args);
        }
        print_terminal(RP);
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_SUBSCRIPT) {
        visit(exp->subscript.array);
        print_terminal(LB);
        visit(exp->subscript.index);
        print_terminal(RB);
        exp->attr_lvalue = true;

    } else if (exp->exp_kind == EXP_T_DOT) {
        visit(exp->dot.exp);
        print_terminal(DOT);
        print_id(exp->dot.id_text);
        exp->attr_lvalue = true;

    } else if (exp->exp_kind == EXP_T_ID) {
        check_undefine_variable(exp, exp->id_text);
        exp->attr_type = retrieve_variable_type(exp->id_text);
        debug("exp->attr_type = %p", exp->attr_type);
        exp->attr_lvalue = true;

    } else if (exp->exp_kind == EXP_T_INT) {
        exp->attr_type = newBasicInt();
        debug("exp->attr_type = %p", exp->attr_type);
        exp->attr_lvalue = false;

    } else if (exp->exp_kind == EXP_T_FLOAT) {
        exp->attr_type = newBasicFloat();
        debug("exp->attr_type = %p", exp->attr_type);
        exp->attr_lvalue = false;

    } else {
        fatal("unknown exp_type");
    }
}

static void visitArgs(void *node) {
    print_this(node);
    Args *args = (Args *)node;
    visit(args->exp);
    if (args->args != NULL) {
        print_terminal(COMMA);
        visit(args->args);
    }
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
