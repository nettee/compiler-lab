%{
#define YYSTYPE void *

#include <stdio.h>
#include <stdlib.h>

#include "lex.yy.c"
#include "ast.h"
%}

/* declared tokens */
%token TYPE
%token ID
%token INT FLOAT
%token STRUCT RETURN IF ELSE WHILE
%token SEMI COMMA ASSIGNOP RELOP
%token PLUS MINUS STAR DIV
%token AND OR
%token DOT
%token NOT
%token LP RP LB RB LC RC

%%

Program : ExtDefList { root = newProgram($1); }
;

ExtDefList : ExtDef ExtDefList { printf("ExtDefList1\n"); }
    | %empty { printf("ExtDefList2\n"); }
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

VarDec : ID
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

Dec : VarDec
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

