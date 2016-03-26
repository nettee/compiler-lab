%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>

#include "syntax.tab.h"

#include "common.h"

int install_id(char *);
int install_int(int);
int install_float(float);

void error();

int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

%}

delim [ \t\n]
ws {delim}+

letter [A-Za-z]
letter_ {letter}|_
digit [0-9]

integer 0|[1-9]{digit}*
oct 0[0-7]+
inv_oct 0{digit}+
hex 0[Xx][0-9A-Fa-f]+
inv_hex 0[Xx]({digit}|{letter})+

com_float {integer}[.]{digit}+
exp_float (({digit}+[.]{digit}*)|([.]{digit}+))([Ee][+-]?{digit}+)
float {com_float}|{exp_float}

id {letter_}({letter_}|{digit})*
inv_id {digit}({letter_}|{digit})*{letter_}({letter_}|{digit})*

%%

{ws} { /* do nothing */ }

{integer} {
    int num = atoi(yytext);
    int index = install_int(num);
    yylval.type_int = index;
    return INT;
}

{oct} {
    int num = (int)strtol(yytext, NULL, 8);
    int index = install_int(num);
    yylval.type_int = index;
    return INT;
}

{inv_oct} {
    /* invalid oct number */
    printf("Error type A at Line %d: Invalid OCT '%s'\n", yylineno, yytext);
    error();
}

{hex} {
    int num = (int)strtol(yytext, NULL, 16);
    int index = install_int(num);
    yylval.type_int = index;
    return INT;
}

{inv_hex} {
    printf("Error type A at Line %d: Invalid HEX '%s'\n",
            yylineno, yytext);
    error();
}

{float} {
    float num = atof(yytext);
    int index = install_float(num);
    yylval.type_int = index;
    return FLOAT;
}

int { 
    yylval.type_int = T_INT;
    return TYPE; 
}

float { 
    yylval.type_int = T_FLOAT;
    return TYPE; 
}

struct { return STRUCT; }
return { return RETURN; }
if { return IF; }
else { return ELSE; }
while { return WHILE; }

{id} { 
    int index = install_id(yytext);
    yylval.type_int = index;
    return ID;
}

{inv_id} {
    /* invalid id */
    printf("Error type A at Line %d: Invalid ID '%s'\n", 
            yylineno, yytext);
    error();
}

";" { return SEMI; }
"," { return COMMA; }
"=" { return ASSIGNOP; }
"<" { yylval.type_int = RELOP_LT; return RELOP; }
"<=" { yylval.type_int = RELOP_LE; return RELOP; }
">" { yylval.type_int = RELOP_GT; return RELOP; }
">=" { yylval.type_int = RELOP_GE; return RELOP; }
"==" { yylval.type_int = RELOP_EQ; return RELOP; }
"!=" { yylval.type_int = RELOP_NE; return RELOP; }
"+" { return PLUS; }
"-" { return MINUS; }
"*" { return STAR; }
"/" { return DIV; }
"&&" { return AND; }
"||" { return OR; }
"." { return DOT; }
"!" { return NOT; }
"(" { return LP; }
")" { return RP; }
"[" { return LB; }
"]" { return RB; }
"{" { return LC; }
"}" { return RC; }

. {
    printf("Error type A at Line %d: Mysterious character '%s'\n",
            yylineno, yytext);
    error();
}

%%

void error() {
    exit(1);   
}
