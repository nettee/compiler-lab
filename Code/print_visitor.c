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
        printf("| "); \
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
    print("%s", token_str(terminal));
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

void visitProgram(void *node) {
    print("Program");
    Program *program = (Program *)node;
    visit(program->extDefList);
}

void visitExtDefList(void *node) {
    /* may produce epsilon */
    ExtDefList *extDefList = (ExtDefList *)node;
    if (extDefList->extDef != NULL) {
        print("ExtDefList");
        visit(extDefList->extDef);
        visit(extDefList->extDefList);
    }
}

void visitExtDef(void *node) {
    print("ExtDef");
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

void visitExtDecList(void *node) {
    print("ExtDecList");
    ExtDecList *extDecList = (ExtDecList *)node;
    visit(extDecList->varDec);
    if (extDecList->extDecList != NULL) {
        print_terminal(COMMA);
        visit(extDecList->extDecList);
    }
}

void visitSpecifier(void *node) {
    print("Specifier");
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

void visitStructSpecifier(void *node) {
    print("StructSpecifier");
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

void visitOptTag(void *node) {
    print("OptTag");
    OptTag *optTag = (OptTag *)node;
    if (optTag->has_id) {
        print_id(optTag->id_index);
    }
}

void visitTag(void *node) {
    print("Tag");
    Tag *tag = (Tag *)node;
    print_id(tag->id_index);
}

void visitVarDec(void *node) {
    print("VarDec");
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

void visitFunDec(void *node) {
    print("FunDec");
    FunDec *funDec = (FunDec *)node;
    print_id(funDec->id_index);
    print_terminal(LP);
    if (funDec->varList != NULL) {
        visit(funDec->varList);
    }
    print_terminal(RP);
}

void visitVarList(void *node) {
    print("VarList");
    VarList *varList = (VarList *)node;
    visit(varList->paramDec);
    if (varList->varList != NULL) {
        print_terminal(COMMA);
        visit(varList->varList);
    }
}

void visitParamDec(void *node) {
    print("ParamDec");
    ParamDec *paramDec = (ParamDec *)node;
    visit(paramDec->specifier);
    visit(paramDec->varDec);
}

void visitCompSt(void *node) {
    print("CompSt");
    CompSt *compSt = (CompSt *)node;
    visit(compSt->defList);
    visit(compSt->stmtList);
}

void visitStmtList(void *node) {
    /* may produce epsilon */
    StmtList *stmtList = (StmtList *)node;
    if (stmtList->stmt != NULL) {
        print("StmtList");
        visit(stmtList->stmt);
        visit(stmtList->stmtList);
    }
}

void visitStmt(void *node) {
    print("Stmt");
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

void visitDefList(void *node) {
    /* may produce epsilon */
    DefList *defList = (DefList *)node;
    if (defList->def != NULL) {
        print("DefList");
        visit(defList->def);
        visit(defList->defList);
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
    visit(decList->dec);
    if (decList->decList != NULL) {
        print_terminal(COMMA);
        visit(decList->decList);
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

void visitArgs(void *node) {
    print("Args");
    Args *args = (Args *)node;
    visit(args->exp);
    if (args->args != NULL) {
        print_terminal(COMMA);
        visit(args->args);
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
