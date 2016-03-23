%{
#include <stdio.h>
#include <stdlib.h>

#include "syntax.tab.h"

#include "common.h"

void error();
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
    //printf("INTEGER %d\n", num);

    return INT;
}

{oct} {
    unsigned int num = (unsigned int)strtol(yytext, NULL, 8);
    //printf("OCT 0%o\n", num);

    return INT;
}

{inv_oct} {
    /* invalid oct number */
    printf("Error type A at Line %d: Invalid OCT '%s'\n", yylineno, yytext);
    error();
}

{hex} {
    unsigned int num = (unsigned int)strtol(yytext, NULL, 16);
    //printf("HEX 0x%x\n", num);

    return INT;
}

{inv_hex} {
    printf("Error type A at Line %d: Invalid HEX '%s'\n",
            yylineno, yytext);
    error();
}

{float} {
    float num = atof(yytext);
    //printf("FLOAT %.3f\n", num);

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
whlie { return WHILE; }

{id} { 
    //printf("ID '%s'\n", yytext); 
    yylval.type_int = 1078;
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
"<" { return RELOP; }
"<=" { return RELOP; }
">" { return RELOP; }
">=" { return RELOP; }
"==" { return RELOP; }
"!=" { return RELOP; }
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
