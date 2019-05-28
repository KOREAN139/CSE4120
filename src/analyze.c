#include "analyze.h"
#include "globals.h"
#include "symtab.h"

static int location;
static int func_loc;
static int scope_level;

/*
 * Function traverse traverses given tree, executes given
 * pre-procedure & post-procedure functions
 */
static void traverse(node_t *t,
                void (*pre_proc)(node_t *),
                void (*post_proc)(node_t *))
{
        int i, local_loc;
        if (t) {
                local_loc = location;
                if (pre_proc)
                        pre_proc(t);
                for (i = 0; i < MAXCHILDREN; ++i)
                        traverse(t->child[i], pre_proc, post_proc);
                if (post_proc)
                        post_proc(t);
                if (scope_level) {
                        scope_level -= 1;
                        parent_symbol_table();
                }
                location = local_loc;
                traverse(t->sibling, pre_proc, post_proc);
        }
}

/*
 * Function update_symtab updates symbol table with current symbol
 * in t if any
 */
static void update_symtab(node_t *t)
{
        int array_size = 0;
        node_t *node_ptr;
        symbol_type_t symbol_type;

        switch (t->nodekind) {
        case StmtK:
                switch (t->kind.stmt) {
                case CompK:
                        child_symbol_table();
                        scope_level += 1;
                        break;
                default:
                        break;
                }
                break;
        case ExprK:
                switch (t->kind.expr) {
                case IdK:
                        if (lookup_symbol(t->attr.name) == -1) {
                                // current variable is not visible in scope
                        }
                        break;
                default:
                        break;
                }
                break;
        case DeclK:
                switch (t->kind.decl) {
                case ArrayK:
                        array_size = t->child[2]->attr.val;
                case VarK:
                        symbol_type = Var;
                        location -= 4 * array_size;
                        if (!array_size)
                                location -= 4;
                        insert_symbol(t->child[1]->attr.name, t->type,
                                        symbol_type, array_size,
                                        t->lineno, location);
                        break;
                case FunK:
                        symbol_type = Func;
                        insert_symbol(t->child[1]->attr.name, t->type,
                                        symbol_type, array_size,
                                        t->lineno, func_loc);
                        func_loc += 1;

                        location = 0;
                        node_ptr = t->child[2];
                        symbol_type = Param;
                        while (node_ptr) {
                                insert_symbol(node_ptr->child[1]->attr.name,
                                                node_ptr->type, symbol_type,
                                                array_size, node_ptr->lineno,
                                                location);
                                location += 4;
                                node_ptr = node_ptr->sibling;
                        }

                        location = -4;
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
        location = 0;
        func_loc = 0;
        scope_level = 0;
        init_symbol_table();
        traverse(tree, update_symtab, NULL);
}
