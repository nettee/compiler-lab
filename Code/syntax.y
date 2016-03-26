%{
#include <stdio.h>
#include <stdlib.h>

#include "lex.yy.c"
#include "ast.h"
%}

/* declared types */
%union {
    int type_int;
    void *type_node;
}

/* declared tokens */
%token <type_int> TYPE
%token <type_int> ID
%token <type_int> INT FLOAT
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

%precedence LOWER_THAN_ELSE
%precedence ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%precedence NEG NOT
%precedence DOT LB


%%

Program : ExtDefList { $$ = root = newProgram($1); }
;

ExtDefList : ExtDef ExtDefList { $$ = newExtDefList($1, $2); }
    | %empty { $$ = newExtDefList(NULL, NULL); }
;

ExtDef : Specifier ExtDecList SEMI 
        { $$ = newExtDef_var($1, $2); }
    | Specifier SEMI { $$ = newExtDef_struct($1); }
    | Specifier FunDec CompSt { $$ = newExtDef_fun($1, $2, $3); }
;

ExtDecList : VarDec { $$ = newExtDecList($1, NULL); }
    | VarDec COMMA ExtDecList { $$ = newExtDecList($1, $3); }
;

Specifier : TYPE { $$ = newSpecifier_TYPE($1); }
    | StructSpecifier { $$ = newSpecifier_struct($1); }
;

StructSpecifier : STRUCT OptTag LC DefList RC
        { $$ = newStructSpecifier_def($2, $4); }
    | STRUCT Tag { $$ = newStructSpecifier_dec($2); }
;

OptTag : ID { $$ = newOptTag($1); }
    | %empty { $$ = newOptTag_empty(); }
;

Tag : ID { $$ = newTag($1); }
;

VarDec : ID { $$ = newVarDec_ID($1); }
    | VarDec LB INT RB { $$ = newVarDec_dim($1, $3); }
;

FunDec : ID LP VarList RP { $$ = newFunDec($1, $3); }
    | ID LP RP { $$ = newFunDec($1, NULL); }
;

VarList : ParamDec COMMA VarList { $$ = newVarList($1, $3); }
    | ParamDec { $$ = newVarList($1, NULL); }
;

ParamDec : Specifier VarDec { $$ = newParamDec($1, $2); }
;

CompSt : LC DefList StmtList RC { $$ = newCompSt($2, $3); }
;

StmtList : Stmt StmtList { $$ = newStmtList($1, $2); }
    | %empty { $$ = newStmtList(NULL, NULL); }
;

Stmt : Exp SEMI { $$ = newStmt_exp($1); }
    | CompSt { $$ = newStmt_COMP_ST($1); }
    | RETURN Exp SEMI { $$ = newStmt_RETURN($2); }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE 
        { $$ = newStmt_if($3, $5); }
    | IF LP Exp RP Stmt ELSE Stmt
        { $$ = newStmt_ifelse($3, $5, $7); }
    | WHILE LP Exp RP Stmt { $$ = newStmt_WHILE($3, $5); }
;

DefList : Def DefList { $$ = newDefList($1, $2); }
    | %empty { $$ = newDefList(NULL, NULL); }
;

Def : Specifier DecList SEMI { $$ = newDef($1, $2); }
;

DecList : Dec { $$ = newDecList($1, NULL); }
    | Dec COMMA DecList { $$ = newDecList($1, $3); }
;

Dec : VarDec { $$ = newDec($1, NULL); }
    | VarDec ASSIGNOP Exp { $$ = newDec($1, $3); }
;

Exp : LP Exp RP { $$ = newExp_paren($2); }
    | ID LP Args RP { $$ = newExp_call($1, $3); }
    | ID LP RP { $$ = newExp_call($1, NULL); }
    | Exp LB Exp RB { $$ = newExp_subscript($1, $3); }
    | Exp DOT ID { $$ = newExp_DOT($1, $3); }

    | MINUS %prec NEG Exp { $$ = newExp_unary(MINUS, $2); }
    | NOT Exp { $$ = newExp_unary(NOT, $2); }

    | Exp STAR Exp { $$ = newExp_infix(STAR, $1, $2, $3); } 
    | Exp DIV Exp { $$ = newExp_infix(DIV, $1, $2, $3); } 

    | Exp PLUS Exp { $$ = newExp_infix(PLUS, $1, $2, $3); }
    | Exp MINUS Exp { $$ = newExp_infix(MINUS, $1, $2, $3); } 

    | Exp RELOP Exp { $$ = newExp_infix(RELOP, $1, $2, $3); } 

    | Exp AND Exp { $$ = newExp_infix(AND, $1, $2, $3); } 

    | Exp OR Exp { $$ = newExp_infix(OR, $1, $2, $3); } 

    | Exp ASSIGNOP Exp { $$ = newExp_infix(ASSIGNOP, $1, $2, $3); }

    | ID { $$ = newExp_ID($1); }
    | INT { $$ = newExp_INT($1); }
    | FLOAT { $$ = newExp_FLOAT($1); }
;

Args : Exp COMMA Args { $$ = newArgs($1, $3); }
    | Exp { $$ = newArgs($1, NULL); }
;


%%

