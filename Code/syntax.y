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

Specifier : TYPE 
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

CompSt : LC DefList StmtList RC
;

StmtList : Stmt StmtList
    | %empty
;

Stmt : Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
;

DefList : Def DefList
    | %empty
;

Def : Specifier DecList SEMI
;

DecList : Dec
    | Dec COMMA DefList
;

Dec : VarDec { root = newDec_1($1); }
    | VarDec ASSIGNOP Exp
;

Exp : Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID 
    | INT
    | FLOAT
;

Args : Exp COMMA Args
    | Exp
;


%%

