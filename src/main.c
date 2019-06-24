#include "globals.h"
#include "util.h"
#include "symtab.h"
#include "analyze.h"

/* allocate global variables */
int lineno = 0;
FILE *source;
FILE *listing;
FILE *code;

/* allocate and set tracing flags */
int TraceScan = FALSE;

token_t get_token(void);

int main(int argc, char *argv[])
{
        node_t *syntax_tree;
        /* source code file name */
        char pgm[120];
        if (argc != 2) {
                fprintf(stderr, "usage: %s <filename>\n", argv[0]);
                exit(1);
        }
        strcpy(pgm, argv[1]);
        if (strchr(pgm, '.') == NULL)
                strcat(pgm, ".tny");
        source = fopen(pgm, "r");
        if (source == NULL) {
                fprintf(stderr, "File %s not found\n", pgm);
                exit(1);
        }
        /* send listing to screen */
        listing = stdout;
        syntax_tree = parse();
        build_symbol_table(syntax_tree);
        if (!Error)
                print_symbol_table(listing);
        semantic_check(syntax_tree);
        /* Do build symbol table, check types */
        fclose(source);
        return 0;
}
