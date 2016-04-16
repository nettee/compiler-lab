%{
#include <stdio.h>
#include <stdlib.h>

#include "lex.yy.c"
#include "pt.h"

int nr_syntax_error = 0;

#define psel(text, lineno) \
    printf("Error type B at Line %d: %s\n", \
                lineno, text)

#define psev(...) psevl(yylineno, __VA_ARGS__)

#define psevl(lineno, ...) { \
    printf("Error type B at Line %d: ", lineno); \
    printf(__VA_ARGS__); \
    printf(" [%d]", __LINE__); \
    printf("\n"); \
}

%}

/* declared types */
%union {
    int type_int;
    float type_float;
    char *type_id;
    void *type_node;
}

/* declared tokens */
%token <type_id> ID
%token <type_int> INT
%token <type_float> FLOAT
%token <type_int> TYPE
%token <type_int> STRUCT RETURN IF ELSE WHILE
%token <type_int> SEMI COMMA ASSIGNOP RELOP
%token <type_int> PLUS MINUS STAR DIV
%token <type_int> AND OR
%token <type_int> DOT
%token <type_int> NOT
%token <type_int> LP RP LB RB LC RC

/* declare type for non-terminals */
%type <type_node> Program
%type <type_node> ExtDefList
%type <type_node> ExtDef
%type <type_node> ExtDecList
%type <type_node> Specifier
%type <type_node> StructSpecifier
%type <type_node> OptTag
%type <type_node> Tag
%type <type_node> VarDec
%type <type_node> FunDec
%type <type_node> VarList
%type <type_node> ParamDec
%type <type_node> CompSt
%type <type_node> StmtList
%type <type_node> Stmt
%type <type_node> DefList
%type <type_node> Def
%type <type_node> DecList
%type <type_node> Dec
%type <type_node> Exp
%type <type_node> Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NEG NOT
%right DOT LB


%%

Program : ExtDefList 
        { $$ = root = newProgram($1, @$.first_line); }
;

ExtDefList : ExtDef ExtDefList 
        { $$ = newExtDefList($1, $2, @$.first_line); }
    |  
        { $$ = newExtDefList(NULL, NULL, @$.first_line); }
;

ExtDef : Specifier ExtDecList SEMI 
        { $$ = newExtDef_var($1, $2, @$.first_line); }
    | Specifier SEMI 
        { $$ = newExtDef_struct($1, @$.first_line); }
    | Specifier FunDec CompSt 
        { $$ = newExtDef_fun($1, $2, $3, @$.first_line); }
    | Specifier error
        { psev("syntax error, near `%s'", yytext); }
;

ExtDecList : VarDec 
        { $$ = newExtDecList($1, NULL, @$.first_line); }
    | VarDec COMMA ExtDecList 
        { $$ = newExtDecList($1, $3, @$.first_line); }
;

Specifier : TYPE 
        { $$ = newSpecifier_basic($1, @$.first_line); }
    | StructSpecifier 
        { $$ = newSpecifier_struct($1, @$.first_line); }
;

StructSpecifier : STRUCT OptTag LC DefList RC
        { $$ = newStructSpecifier_def($2, $4, @$.first_line); }
    | STRUCT Tag 
        { $$ = newStructSpecifier_dec($2, @$.first_line); }
;

OptTag : ID 
        { $$ = newOptTag($1, @$.first_line); }
    |  
        { $$ = newOptTag(NULL, @$.first_line); }
;

Tag : ID 
        { $$ = newTag($1, @$.first_line); }
;

VarDec : ID 
        { $$ = newVarDec_ID($1, @$.first_line); }
    | VarDec LB INT RB 
        { $$ = newVarDec_dim($1, $3, @$.first_line); }
    | VarDec LB error RB
        { psev("Invalid variable dimension"); }
;

FunDec : ID LP VarList RP 
        { $$ = newFunDec($1, $3, @$.first_line); }
    | ID LP RP 
        { $$ = newFunDec($1, NULL, @$.first_line); }
    | ID error
        { psev("syntax error"); }
;

VarList : ParamDec COMMA VarList 
        { $$ = newVarList($1, $3, @$.first_line); }
    | ParamDec 
        { $$ = newVarList($1, NULL, @$.first_line); }
;

ParamDec : Specifier VarDec 
        { $$ = newParamDec($1, $2, @$.first_line); }
;

CompSt : LC DefList StmtList RC 
        { $$ = newCompSt($2, $3, @$.first_line); }
    | error RC
        { psev("syntax error, near `%s'", yytext); }
;

StmtList : Stmt StmtList 
        { $$ = newStmtList($1, $2, @$.first_line); }
    |  
        { $$ = newStmtList(NULL, NULL, @$.first_line); }
;

Stmt : Exp SEMI 
        { $$ = newStmt_exp($1, @$.first_line); }
    | error SEMI
        { psev("Invalid expression"); }
    | error '\n'
        { psev("syntax error, near `%s'", yytext); }
    | CompSt 
        { $$ = newStmt_COMP_ST($1, @$.first_line); }
    | RETURN Exp SEMI 
        { $$ = newStmt_RETURN($2, @$.first_line); }
    | RETURN error SEMI
        { psev("Invalid expression"); }
    | RETURN Exp error
        { psev("syntax error, near `%s'", yytext); }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE 
        { $$ = newStmt_if($3, $5, @$.first_line); }
    | IF error Stmt %prec LOWER_THAN_ELSE
        { psel("Invalid expression after 'if'", @$.first_line); }
    | IF LP Exp RP Stmt ELSE Stmt
        { $$ = newStmt_ifelse($3, $5, $7, @$.first_line); }
    | IF error Stmt ELSE Stmt
        { psel("Invalid expression after 'if'", @$.first_line); }
    | WHILE LP Exp RP Stmt 
        { $$ = newStmt_WHILE($3, $5, @$.first_line); }
    | WHILE error Stmt
        { psel("Invalid expression after 'while'", @$.first_line); }

;

DefList : Def DefList 
        { $$ = newDefList($1, $2, @$.first_line); }
    |  
        { $$ = newDefList(NULL, NULL, @$.first_line); }
;

Def : Specifier DecList SEMI 
        { $$ = newDef($1, $2, @$.first_line); }
    | Specifier error SEMI
        { psev("Invalid definition"); }
    | Specifier DecList error
        { psev("syntax error, near `%s'", yytext); }
;

DecList : Dec 
        { $$ = newDecList($1, NULL, @$.first_line); }
    | Dec COMMA DecList 
        { $$ = newDecList($1, $3, @$.first_line); }
;

Dec : VarDec 
        { $$ = newDec($1, NULL, @$.first_line); }
    | VarDec ASSIGNOP Exp 
        { $$ = newDec($1, $3, @$.first_line); }
;

Exp : LP Exp RP 
        { $$ = newExp_paren($2, @$.first_line); }
    | ID LP Args RP 
        { $$ = newExp_call($1, $3, @$.first_line); }
    | ID LP error RP
        { psev("Invalid arguments"); }
    | ID LP RP 
        { $$ = newExp_call($1, NULL, @$.first_line); }
    | Exp LB Exp RB 
        { $$ = newExp_subscript($1, $3, @$.first_line); }
    | Exp LB error RB
        { psev("Invalid array subscript"); }
    | Exp DOT ID 
        { $$ = newExp_DOT($1, $3, @$.first_line); }

    | MINUS %prec NEG Exp 
        { $$ = newExp_unary(MINUS, $2, @$.first_line); }
    | NOT Exp 
        { $$ = newExp_unary(NOT, $2, @$.first_line); }

    | Exp STAR Exp 
        { $$ = newExp_infix(STAR, $1, $2, $3, @$.first_line); } 
    | Exp DIV Exp 
        { $$ = newExp_infix(DIV, $1, $2, $3, @$.first_line); } 

    | Exp PLUS Exp 
        { $$ = newExp_infix(PLUS, $1, $2, $3, @$.first_line); }
    | Exp MINUS Exp 
        { $$ = newExp_infix(MINUS, $1, $2, $3, @$.first_line); } 

    | Exp RELOP Exp 
        { $$ = newExp_infix(RELOP, $1, $2, $3, @$.first_line); } 

    | Exp AND Exp 
        { $$ = newExp_infix(AND, $1, $2, $3, @$.first_line); } 

    | Exp OR Exp 
        { $$ = newExp_infix(OR, $1, $2, $3, @$.first_line); } 

    | Exp ASSIGNOP Exp 
        { $$ = newExp_infix(ASSIGNOP, $1, $2, $3, @$.first_line); }

    | ID 
        { $$ = newExp_ID($1, @$.first_line); }
    | INT 
        { $$ = newExp_INT($1, @$.first_line); }
    | FLOAT 
        { $$ = newExp_FLOAT($1, @$.first_line); }
;

Args : Exp COMMA Args 
        { $$ = newArgs($1, $3, @$.first_line); }
    | Exp 
        { $$ = newArgs($1, NULL, @$.first_line); }
    | Exp error
        { psev("syntax error, near `%s'", yytext); }
;


%%

int yyerror(char *s) {
    //printf("error at line %d\n", yylineno);
    nr_syntax_error++;
}
