#include "analyze.h"
#include "globals.h"
#include "symtab.h"

extern int Error;

static int scope_level;
static int func_loc;
static int func_comp;

static void print_error_msg(char *name, int lineno)
{
        printf("Error in line %d : ", lineno);
        printf("duplicated declaration of %s", name);
        printf("\n\t\tfirst declared at line %d\n", lineno);
}

/*
 * Function update_symtab updates symbol table with current symbol
 * in t if any
 */
static void _build_symbol_table(node_t *t)
{
        if (!t || Error)
                return;

        int i, diff, location, ret;
        int is_array = 0;
        int array_size = 0;
        node_t *node_ptr;
        symbol_type_t symbol_type;

        switch (t->nodekind) {
        case StmtK:
                switch (t->kind.stmt) {
                case CompK:
                        if (!func_comp) {
                                location = get_memory_location();
                                child_symbol_table();
                                update_memory_location(location);
                                scope_level += 1;
                        }
                        func_comp = 0;
                        for (i = 0; i < MAXCHILDREN; i++)
                                _build_symbol_table(t->child[i]);
                        parent_symbol_table();
                        scope_level -= 1;
                        break;
                default:
                        for (i = 0; i < MAXCHILDREN; i++)
                                _build_symbol_table(t->child[i]);
                        _build_symbol_table(t->sibling);
                        break;
                }
                break;
        case ExprK:
                switch (t->kind.expr) {
                case IdK:
                        if (!lookup_symbol(t->attr.name)) {
                                // current variable is not visible in scope
                                Error = TRUE;
                                printf("Error in line %d : %s not exist\n",
                                                t->lineno, t->attr.name);
                                break;
                        }
                        add_symbol_line(t->attr.name, t->lineno);
                        break;
                default:
                        for (i = 0; i < MAXCHILDREN; i++)
                                _build_symbol_table(t->child[i]);
                        _build_symbol_table(t->sibling);
                        break;
                }
                break;
        case DeclK:
                location = get_memory_location();
                switch (t->kind.decl) {
                case ArrayK:
                        is_array = TRUE;
                        array_size = t->child[2]->attr.val;
                case VarK:
                        symbol_type = Var;
                        diff = 4 * (array_size + !array_size);
                        location += scope_level ? -diff : diff;
                        if (insert_symbol(t->child[1]->attr.name, t->type,
                                        symbol_type, is_array, array_size,
                                        t->lineno, location)) {
                                Error = TRUE;
                                print_error_msg(t->child[1]->attr.name,
                                                t->lineno);
                                return;
                        }

                        update_memory_location(location);
                        _build_symbol_table(t->sibling);
                        break;
                case FunK:
                        symbol_type = Func;
                        if (insert_symbol(t->child[1]->attr.name, t->type,
                                        symbol_type, is_array, array_size,
                                        t->lineno, func_loc)) {
                                Error = TRUE;
                                print_error_msg(t->child[1]->attr.name,
                                                t->lineno);
                                return;
                        }

                        func_loc += 1;

                        func_comp = 1;
                        child_symbol_table();
                        scope_level += 1;

                        location = 0;
                        node_ptr = t->child[2];
                        while (node_ptr) {
                                location += 4;
                                node_ptr = node_ptr->sibling;
                        }

                        symbol_type = Param;
                        node_ptr = t->child[2];
                        while (node_ptr) {
                                if (insert_symbol(node_ptr->child[1]->attr.name,
                                                node_ptr->type, symbol_type,
                                                node_ptr->kind.decl == ArrayK,
                                                array_size, node_ptr->lineno,
                                                location)) {
                                        Error = TRUE;
                                        print_error_msg(t->child[1]->attr.name,
                                                        t->lineno);
                                        return;
                                }

                                location -= 4;
                                node_ptr = node_ptr->sibling;
                        }

                        location = -4;
                        update_memory_location(location);

                        _build_symbol_table(t->child[3]);
                        _build_symbol_table(t->sibling);
                        break;
                default:
                        break;
                }
                break;
        default:
                break;
        }
}

void build_symbol_table(node_t *tree)
{
        scope_level = 0;
        func_loc = 0;
        func_comp = 0;
        init_symbol_table();
        _build_symbol_table(tree);
}

void semantic_check(node_t *tree)
{
}
