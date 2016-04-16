#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "pt.h"
#include "syntax.tab.h"

int nr_semantics_error = 0;

typedef void (*funcptr)(void *);

static void visit(void *node);

char *resolve_id(int);
int resolve_int(int);
float resolve_float(int);

void print_symbol_table();

char *terminal_str(int token_enum);
char *nonterminal_str(int nonterminal_enum);

static int indent = -1;

#define print(...) { \
    int i; \
    for (i = 0; i < indent; i++) { \
        printf("  "); \
    } \
    printf(__VA_ARGS__); \
    printf("\n"); \
}

static void print_id(int id_index) {
    indent++;
    print("ID: %s", resolve_id(id_index));
    indent--;
}

static void print_int(int int_index) {
    indent++;
    print("INT: %d", resolve_int(int_index));
    indent--;
}

static void print_float(int float_index) {
    indent++;
    print("FLOAT: %f", resolve_float(float_index));
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

static void pre_visit(void *node) {
    int type = *(int *)node;
    int lineno = ((int *)node)[1];
    print("%s (%d)", nonterminal_str(type), lineno);
}

static void visitProgram(void *node) {
    pre_visit(node);
    Program *program = (Program *)node;
    visit(program->extDefList);
}

static void visitExtDefList(void *node) {
    /* may produce epsilon */
    ExtDefList *extDefList = (ExtDefList *)node;
    if (extDefList->extDef != NULL) {
        pre_visit(node);
        visit(extDefList->extDef);
        visit(extDefList->extDefList);
    }
}

static void visitExtDef(void *node) {
    pre_visit(node);
    ExtDef *extDef = (ExtDef *)node;
    switch (extDef->extdef_type) {
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
    pre_visit(node);
    ExtDecList *extDecList = (ExtDecList *)node;
    visit(extDecList->varDec);
    if (extDecList->extDecList != NULL) {
        print_terminal(COMMA);
        visit(extDecList->extDecList);
    }
}

static void visitSpecifier(void *node) {
    pre_visit(node);
    Specifier *specifier = (Specifier *)node;
    switch (specifier->specifier_type) {
    case SPECIFIER_T_TYPE:
        print_type(specifier->type_index); 
        break;
    case SPECIFIER_T_STRUCT:
        visit(specifier->structSpecifier);
        break;
    default:
        printf("fatal: unknown specifier_type\n");
    }
}

static void visitStructSpecifier(void *node) {
    pre_visit(node);
    StructSpecifier *structSpecifier = (StructSpecifier *)node;
    switch (structSpecifier->structspecifier_type) {
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
    if (optTag->id_index != -1) {
        pre_visit(node);
        print_id(optTag->id_index);
    }
}

static void visitTag(void *node) {
    pre_visit(node);
    Tag *tag = (Tag *)node;
    print_id(tag->id_index);
}

static void visitVarDec(void *node) {
    pre_visit(node);
    VarDec *varDec = (VarDec *)node;
    switch (varDec->vardec_type) {
    case VAR_DEC_T_ID:
        print_id(varDec->id_index);
        break;
    case VAR_DEC_T_DIM:
        visit(varDec->dim.varDec);
        print_terminal(LB);
        print_int(varDec->dim.int_index);
        print_terminal(RB);
        break;
    default:
        printf("fatal: unknown vardec_type\n");
    }
}

static void visitFunDec(void *node) {
    pre_visit(node);
    FunDec *funDec = (FunDec *)node;
    print_id(funDec->id_index);
    print_terminal(LP);
    if (funDec->varList != NULL) {
        visit(funDec->varList);
    }
    print_terminal(RP);
}

static void visitVarList(void *node) {
    pre_visit(node);
    VarList *varList = (VarList *)node;
    visit(varList->paramDec);
    if (varList->varList != NULL) {
        print_terminal(COMMA);
        visit(varList->varList);
    }
}

static void visitParamDec(void *node) {
    pre_visit(node);
    ParamDec *paramDec = (ParamDec *)node;
    visit(paramDec->specifier);
    visit(paramDec->varDec);
}

static void visitCompSt(void *node) {
    pre_visit(node);
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
        pre_visit(node);
        visit(stmtList->stmt);
        visit(stmtList->stmtList);
    }
}

static void visitStmt(void *node) {
    pre_visit(node);
    Stmt *stmt = (Stmt *)node;
    switch (stmt->stmt_type) {
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
        pre_visit(node);
        visit(defList->def);
        visit(defList->defList);
    }
}

static void visitDef(void *node) {
    pre_visit(node);
    Def *def = (Def *)node;
    visit(def->specifier);
    visit(def->decList);
    print_terminal(SEMI);
}

static void visitDecList(void *node) {
    pre_visit(node);
    DecList *decList = (DecList *)node;
    visit(decList->dec);
    if (decList->decList != NULL) {
        print_terminal(COMMA);
        visit(decList->decList);
    }
}

static void visitDec(void *node) {
    pre_visit(node);
    Dec *dec = (Dec *)node;
    visit(dec->varDec);
    if (dec->exp != NULL) {
        print_terminal(ASSIGNOP);
        visit(dec->exp);
    }
}

static void visitExp(void *node) {
    pre_visit(node);
    Exp *exp = (Exp *)node;
    switch (exp->exp_type) {
    case EXP_T_INFIX:
        visit(exp->infix.exp_left);
        if (exp->infix.op == RELOP) {
            print_relop(exp->infix.op_yylval);
        } else {
            print_terminal(exp->infix.op);
        }
        visit(exp->infix.exp_right);
        break;
    case EXP_T_PAREN:
        print_terminal(LP);
        visit(exp->paren.exp);
        print_terminal(RP);
        break;
    case EXP_T_UNARY:
        print_terminal(exp->unary.op);
        visit(exp->unary.exp);
        break;
    case EXP_T_CALL:
        print_id(exp->call.id_index);
        print_terminal(LP);
        if (exp->call.args != NULL) {
            visit(exp->call.args);
        }
        print_terminal(RP);
        break;
    case EXP_T_SUBSCRIPT:
        visit(exp->subscript.array);
        print_terminal(LB);
        visit(exp->subscript.index);
        print_terminal(RB);
        break;
    case EXP_T_DOT:
        visit(exp->dot.exp);
        print_terminal(DOT);
        print_id(exp->dot.id_index);
        break;
    case EXP_T_ID:
        print_id(exp->id_index);
        break;
    case EXP_T_INT:
        print_int(exp->int_index);
        break;
    case EXP_T_FLOAT:
        print_float(exp->float_index);
        break;
    default:
        printf("fatal: unknown exp_type\n");
    }
}

static void visitArgs(void *node) {
    pre_visit(node);
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

