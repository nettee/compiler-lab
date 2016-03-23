#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "ast.h"
#include "syntax.tab.h"

typedef void (*funcptr)(void *);

void visit(void *node);

char *resolve_id(int);
int resolve_int(int);
float resolve_float(int);

void print_symbol_table();

char *token_str(int token_enum) {
    switch (token_enum) {
        case TYPE: return "TYPE";
        case ID: return "ID";
        case INT: return "INT";
        case FLOAT: return "FLOAT";
        case STRUCT: return "STRUCT";
        case RETURN: return "RETURN";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case SEMI: return "SEMI";
        case COMMA: return "COMMA";
        case ASSIGNOP: return "ASSIGNOP";
        case RELOP: return "RELOP";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case STAR: return "STAR";
        case DIV: return "DIV";
        case AND: return "AND";
        case OR: return "OR";
        case DOT: return "DOT";
        case NOT: return "NOT";
        case LP: return "LP";
        case RP: return "RP";
        case LB: return "LB";
        case RB: return "RB";
        case LC: return "LC";
        case RC: return "RC";
        default:
            printf("fatal: unknown token enum\n");
            return "";
    }
}

static int indent = -1;

#define print(...) { \
    int i; \
    for (i = 0; i < indent; i++) { \
        printf("  "); \
    } \
    printf(__VA_ARGS__); \
    printf("\n"); \
}

void visitProgram(void *node) {
    print("Program");
}

void visitExtDefList(void *node) {
    print("ExtDefList");
}

void visitExtDef(void *node) {
    print("ExtDef");
}

void visitExtDecList(void *node) {
    print("ExtDecList");
}

void visitSpecifier(void *node) {
    print("Specifier");
    Specifier *specifier = (Specifier *)node;
    switch (specifier->type_value) {
    case T_INT:
        print("  TYPE: int");
        break;
    case T_FLOAT:
        print("  TYPE: float");
        break;
    default:
        printf("Fatal: Unknown specifier->type_value\n");
    }
}

void visitStructSpecifier(void *node) {
    print("StructSpecifier");
}

void visitOptTag(void *node) {
    print("OptTag");
}

void visitTag(void *node) {
    print("Tag");
}

void visitVarDec(void *node) {
    print("VarDec");
    VarDec *varDec = (VarDec *)node;
    print("  ID: %s", resolve_id(varDec->id_index));
}

void visitFunDec(void *node) {
    print("FunDec");
}

void visitVarList(void *node) {
    print("VarList");
}

void visitParamDec(void *node) {
    print("ParamDec");
}

void visitCompSt(void *node) {
    print("CompSt");
    CompSt *compSt = (CompSt *)node;
    visit(compSt->defList);
    visit(compSt->stmtList);
}

void visitStmtList(void *node) {
    print("StmtList");
    StmtList *stmtList = (StmtList *)node;
    for (ListNode *q = stmtList->list_stmt; q != NULL; q = q->next) {
        visit(q->child);
    }
}

void visitStmt(void *node) {
    print("Stmt");
    Stmt *stmt = (Stmt *)node;
    switch (stmt->stmt_type) {
    case STMT_T_RETURN:
        visit(stmt->exp);
        break;
    default:
        printf("fatal: unknown stmt_type\n");
    }
}

void visitDefList(void *node) {
    print("DefList");
    DefList *defList = (DefList *)node;
    for (ListNode *q = defList->list_def; q != NULL; q = q->next) {
        visit(q->child);
    }
}

void visitDef(void *node) {
    print("Def");
    Def *def = (Def *)node;
    visit(def->specifier);
    visit(def->decList);
}

void visitDecList(void *node) {
    print("DecList");
    DecList *decList = (DecList *)node;
    for (ListNode *q = decList->list_dec; q != NULL; q = q->next) {
        visit(q->child);
    }
}

void visitDec(void *node) {
    print("Dec");
    Dec *dec = (Dec *)node;
    visit(dec->varDec);
}

void visitExp(void *node) {
    print("Exp");
    Exp *exp = (Exp *)node;
    switch (exp->exp_type) {
    case EXP_T_INFIX:
        visit(exp->exp_left);
        print("  %s", token_str(exp->op));
        visit(exp->exp_right);
        break;
    case EXP_T_ID:
        print("  ID: %s", resolve_id(exp->id_index));
        break;
    case EXP_T_INT:
        print("  INT: %d", resolve_int(exp->int_index));
        break;
    case EXP_T_FLOAT:
        print("  FLOAT: %f", resolve_float(exp->float_index));
        break;
    default:
        printf("fatal: unknown exp_type\n");
    }
}

void visitArgs(void *node) {
    print("Args");
}

funcptr visitor_table[] = {
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

void visit(void *node) {
    //printf("node = %p\n", node);
    int type = *(int *)node;
    //printf("type = %d\n", type);
    indent++;
    visitor_table[type-400](node);
    indent--;
}

void print_ast() {
    print_symbol_table();
    visit(root);
}
