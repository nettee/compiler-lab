#include <stdio.h>

#include "common.h"

//#define PRINT_AST
#define SEMANTICS_ANALYSIS
#define IR_GENERATE

extern int nr_lexical_error;
extern int nr_syntax_error;
extern int nr_semantics_error;

void yyrestart(FILE *);
int yyparse(void);

void print_ast();
void init_env();
void semantics_analysis();
void print_symbol_table();
void generate_intercode();

FILE *ir_out_file;
FILE *ir_out_file2;

int main(int argc, char **argv)
{
    if (argc <= 1) {
        fprintf(stderr, "Fatal: too few arguments\n");
        return 1;
    }
    ir_out_file = fopen("/home/dell/a.ir", "w");
    ir_out_file2 = fopen("/home/dell/b.ir", "w");
    if (!ir_out_file) {
        fprintf(stderr, "Fatal: cannot open %s\n",
                "/home/dell/a.ir");
    }
    if (!ir_out_file2) {
        fprintf(stderr, "Fatal: cannot open %s\n",
                "/home/dell/b.ir");
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
    init_env();
    semantics_analysis();

#ifdef NETTEE_DEBUG
    print_symbol_table();
#endif

    if (nr_semantics_error > 0) {
        return 2;
    }
#endif

#ifdef IR_GENERATE
    generate_intercode();
#endif


    return 0;
}
