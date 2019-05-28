#include "analyze.h"
#include "globals.h"
#include "symtab.h"

extern int Error;

static int scope_level;
static int func_loc;
static int func_comp;

/*
 * Function update_symtab updates symbol table with current symbol
 * in t if any
 */
static void _build_symbol_table(node_t *t)
{
        if (!t || Error)
                return;

        int i, diff, location;
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
                        if (lookup_symbol(t->attr.name) == -1) {
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
                if (lookup_symbol(t->child[1]->attr.name) != -1) {
                        Error = TRUE;
                        printf("Error in line %d : duplicated declaration",
                                        t->lineno);
                        printf(" of %s\n\t\tfirst declared at line %d\n",
                                        t->child[1]->attr.name, t->lineno);
                        return;
                }
                switch (t->kind.decl) {
                case ArrayK:
                        array_size = t->child[2]->attr.val;
                case VarK:
                        symbol_type = Var;
                        diff = 4 * (array_size + !array_size);
                        location += scope_level ? -diff : diff;
                        insert_symbol(t->child[1]->attr.name, t->type,
                                        symbol_type, array_size,
                                        t->lineno, location);

                        update_memory_location(location);
                        _build_symbol_table(t->sibling);
                        break;
                case FunK:
                        symbol_type = Func;
                        insert_symbol(t->child[1]->attr.name, t->type,
                                        symbol_type, array_size,
                                        t->lineno, func_loc);
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
                                insert_symbol(node_ptr->child[1]->attr.name,
                                                node_ptr->type, symbol_type,
                                                array_size, node_ptr->lineno,
                                                location);
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
