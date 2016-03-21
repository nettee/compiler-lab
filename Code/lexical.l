%{

%}

delim [ \t\n]
ws {delim}+

letter [A-Za-z]
letter_ {letter}|_
digit [0-9]

integer 0|[1-9]{digit}*
oct 0[1-7][0-7]*
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
    printf("INTEGER %d\n", num);
}

{oct} {
    unsigned int num = (unsigned int)strtol(yytext, NULL, 8);
    printf("OCT 0%o\n", num);
}

{inv_oct} {
    /* invalid oct number */
    printf("Error type A at Line %d: Invalid OCT '%s'\n",
            yylineno, yytext);
}

{hex} {
    unsigned int num = (unsigned int)strtol(yytext, NULL, 16);
    printf("HEX 0x%x\n", num);
}

{inv_hex} {
    printf("Error type A at Line %d: Invalid HEX '%s'\n",
            yylineno, yytext);
}

{float} {
    float num = atof(yytext);
    printf("FLOAT %.3f\n", num);
}

int { printf("TYPE int\n"); }
float { printf("TYPE float\n"); }

struct { printf("STRUCT\n"); }
return { printf("RETURN\n"); }
if { printf("IF\n"); }
else { printf("ELSE\n"); }
whlie { printf("WHLIE"); }

{id} { 
    printf("ID '%s'\n", yytext); 
}

{inv_id} {
    /* invalid id */
    printf("Error type A at Line %d: Invalid ID '%s'\n", 
            yylineno, yytext);
}

";" { printf("SEMI\n"); }
"," { printf("COMMA\n"); }
"=" { printf("ASSIGNOP\n"); }
"<" { printf("RELOP-LT\n"); }
"<=" { printf("RELOP-LE\n"); }
">" { printf("RELOP-GT\n"); }
">=" { printf("RELOP-GE\n"); }
"==" { printf("RELOP-EQ\n"); }
"!=" { printf("RELOP-NE\n"); }
"+" { printf("PLUS\n"); }
"-" { printf("MINUS\n"); }
"*" { printf("STAR\n"); }
"/" { printf("DIV\n"); }
"&&" { printf("AND\n"); }
"||" { printf("OR\n"); }
"." { printf("DOT\n"); }
"!" { printf("NOT\n"); }
"(" { printf("LP\n"); }
")" { printf("RP\n"); }
"[" { printf("LB\n"); }
"]" { printf("RB\n"); }
"{" { printf("LC\n"); }
"}" { printf("RC\n"); }

. {
    printf("Error type A at Line %d: Mysterious character '%s'\n",
            yylineno, yytext);
}

%%

int main(int argc, char **argv) {
    yylex();
    return 0;
}
