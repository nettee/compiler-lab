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

int nr_lexical_error = 0;

int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

%}

newline [\n]
delim [ \t]
ws {delim}+

letter [A-Za-z]
letter_ {letter}|_
digit [0-9]

integer 0|[1-9]{digit}*
oct 0[0-7]+
inv_oct 0{digit}+
hex 0[Xx][0-9A-Fa-f]+
inv_hex 0[Xx]({digit}|{letter})+

float {integer}[.]{digit}+

id {letter_}({letter_}|{digit})*
inv_id {digit}({letter_}|{digit})*{letter_}({letter_}|{digit})*

%%

{newline} { yycolumn = 1; }
{ws} { /* do nothing */ }

{integer} {
    int int_value = atoi(yytext);
    yylval.type_int = int_value;
    return INT;
}

{oct} {
    int int_value = (int)strtol(yytext, NULL, 8);
    yylval.type_int = int_value;
    return INT;
}

{inv_oct} {
    /* invalid oct number */
    printf("Error type A at Line %d: Illegal octal number '%s'\n", yylineno, yytext);
    error();
    yylval.type_int = 0;
    return INT;
}

{hex} {
    int int_value = (int)strtol(yytext, NULL, 16);
    yylval.type_int = int_value;
    return INT;
}

{inv_hex} {
    printf("Error type A at Line %d: Illegal hexadecimal number '%s'\n",
            yylineno, yytext);
    error();
    yylval.type_int = 0;
    return INT;
}

{float} {
    float float_value = atof(yytext);
    yylval.type_float = float_value;
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
    yylval.type_id = strdup_(yytext);
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
    nr_lexical_error++;
    //exit(1);   
}
