#include <stdio.h>

extern int nr_lexical_error;
extern int nr_syntax_error;

void print_ast();

void yyrestart(FILE *);
int yyparse(void);

int main(int argc, char **argv)
{
    if (argc <= 1) {
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();

    if (nr_lexical_error == 0 && nr_syntax_error == 0) {
        print_ast();
    }

    return 0;
}
