#include <stdio.h>

#define PRINT_AST
//#define SEMANTICS_ANALYSIS

extern int nr_lexical_error;
extern int nr_syntax_error;
extern int nr_semantics_error;

void yyrestart(FILE *);
int yyparse(void);

void print_ast();
void semantics_analysis();

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

    if (nr_lexical_error > 0 || nr_syntax_error > 0) {
        return 1;
    }

#ifdef PRINT_AST
    print_ast();
#endif

#ifdef SEMANTICS_ANALYSIS
    semantics_analysis();

    if (nr_semantics_error > 0) {
        return 2;
    }
#endif


    return 0;
}
