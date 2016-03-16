%{

%}

delim [ \t\n]
ws {delim}+
letter_ [A-Za-z]
digit [0-9]
id {letter_}({letter_}|{digit})*
number {digit}+(\.{digit}?)?(E[+-]?{digit}+)?

%%

{ws} { /* do nothing */ }

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

{number} {
    printf("NUMBER '%s'\n", yytext);
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
