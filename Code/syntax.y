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

%%

Program : ExtDefList
;

ExtDefList : ExtDef ExtDefList 
    | %empty 
;

ExtDef : Specifier ExtDecList SEMI 
    | Specifier SEMI 
    | Specifier FunDec CompSt 
;

ExtDecList : VarDec 
    | VarDec COMMA ExtDecList 
;

Specifier : TYPE { $$ = newSpecifier_1($1); }
    | StructSpecifier
;

StructSpecifier : STRUCT OptTag LC DefList RC
    | STRUCT Tag
;

OptTag : ID
    | %empty
;

Tag : ID
;

VarDec : ID { $$ = newVarDec_1($1); }
    | VarDec LB INT RB
;

FunDec : ID LP VarList RP
    | ID LP RP
;

VarList : ParamDec COMMA VarList
    | ParamDec
;

ParamDec : Specifier VarDec
;

CompSt : LC DefList StmtList RC { $$ = root = newCompSt($2, $3); }
;

StmtList : Stmt StmtList { $$ = StmtList_insert($1, $2); }
    | %empty { $$ = newStmtList(); }
;

Stmt : Exp SEMI { $$ = newStmt_exp($1); }
    | CompSt { $$ = newStmt_COMP_ST($1); }
    | RETURN Exp SEMI { $$ = newStmt_RETURN($2); }
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt { $$ = newStmt_WHILE($3, $5); }
;

DefList : Def DefList { $$ = DefList_insert($1, $2); }
    | %empty { $$ = newDefList(); }
;

Def : Specifier DecList SEMI { $$ = newDef($1, $2); }
;

DecList : Dec { $$ = newDecList($1); }
    | Dec COMMA DecList { $$ = DecList_insert($1, $3); }
;

Dec : VarDec { $$ = newDec($1, NULL); }
    | VarDec ASSIGNOP Exp { $$ = newDec($1, $3); }
;

Exp : Exp ASSIGNOP Exp { $$ = newExp_infix(ASSIGNOP, $1, $3); }
    | Exp AND Exp { $$ = newExp_infix(AND, $1, $3); } 
    | Exp OR Exp { $$ = newExp_infix(OR, $1, $3); } 
    | Exp RELOP Exp { $$ = newExp_infix(RELOP, $1, $3); } 
    | Exp PLUS Exp { $$ = newExp_infix(PLUS, $1, $3); }
    | Exp MINUS Exp { $$ = newExp_infix(MINUS, $1, $3); } 
    | Exp STAR Exp { $$ = newExp_infix(STAR, $1, $3); } 
    | Exp DIV Exp { $$ = newExp_infix(DIV, $1, $3); } 
    | LP Exp RP { $$ = newExp_paren($2); }
    | MINUS Exp { $$ = newExp_unary(MINUS, $2); }
    | NOT Exp { $$ = newExp_unary(NOT, $2); }
    | ID LP Args RP { $$ = newExp_call($1, $3); }
    | ID LP RP { $$ = newExp_call($1, NULL); }
    | Exp LB Exp RB { $$ = newExp_subscript($1, $3); }
    | Exp DOT ID { $$ = newExp_DOT($1, $3); }
    | ID { $$ = newExp_ID($1); }
    | INT { $$ = newExp_INT($1); }
    | FLOAT { $$ = newExp_FLOAT($1); }
;

Args : Exp COMMA Args { $$ = Args_insert($1, $3); }
    | Exp { $$ = newArgs($1); }
;


%%

