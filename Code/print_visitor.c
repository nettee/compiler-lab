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

static void print_id(int id_index) {
    print("  ID: %s", resolve_id(id_index));
}

static void print_terminal(int terminal) {
    print("  %s", token_str(terminal));
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
    case STMT_T_EXP:
        visit(stmt->exp.exp);
        print("  %s", token_str(SEMI));
        break;
    case STMT_T_COMP_ST:
        visit(stmt->compst.compSt);
        break;
    case STMT_T_RETURN:
        print("  %s", token_str(RETURN));
        visit(stmt->return_.exp);
        print("  %s", token_str(SEMI));
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
    if (dec->exp != NULL) {
        print_terminal(ASSIGNOP);
        visit(dec->exp);
    }
}

void visitExp(void *node) {
    print("Exp");
    Exp *exp = (Exp *)node;
    switch (exp->exp_type) {
    case EXP_T_INFIX:
        visit(exp->infix.exp_left);
        print("  %s", token_str(exp->infix.op));
        visit(exp->infix.exp_right);
        break;
    case EXP_T_PAREN:
        print("  %s", token_str(LP));
        visit(exp->paren.exp);
        print("  %s", token_str(RP));
        break;
    case EXP_T_UNARY:
        print("  %s", token_str(exp->unary.op));
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
        print("  %s", token_str(LB));
        visit(exp->subscript.index);
        print("  %s", token_str(RB));
        break;
    case EXP_T_DOT:
        visit(exp->dot.exp);
        print("  %s", token_str(DOT));
        print_id(exp->dot.id_index);
        break;
    case EXP_T_ID:
        print_id(exp->id_index);
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
    Args *args = (Args *)node;
    for (ListNode *q = args->list_exp; q != NULL; q = q->next) {
        visit(q->child);
    }
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
