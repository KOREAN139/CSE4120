#include "analyze.h"
#include "globals.h"

static int location;

/*
 * Function traverse traverses given tree, executes given
 * pre-procedure & post-procedure functions
 */
static void traverse(node_t *t,
                void (*pre_proc)(node_t *),
                void (*post_proc)(node_t *))
{
        if (t) {
                if (pre_proc)
                        pre_proc(t);
                for (i = 0; i < MAXCHILDREN; ++i)
                        traverse(t->child[i], pre_proc, post_proc);
                if (post_proc)
                        post_proc(t);
                traverse(t->sibling, pre_proc, post_proc);
        }
}

/*
 * Function update_symtab updates symbol table with current symbol
 * in t if any
 */
static void update_symtab(node_t *t)
{
        switch (t->nodekind) {
        case stmt_t:
                switch (t->kind.stmt) {
                case IfK:
                        break;
                case ReturnK:
                        break;
                case WhileK:
                        break;
                case CompK:
                        break;
                default:
                        break;
                }
                break;
        case expr_t:
                switch (t->kind.expr) {
                case OpK:
                        break;
                case ConstK:
                        break;
                case IdK:
                        break;
                case TypeK:
                        break;
                case FunCallK:
                        break;
                case ArrSubK:
                        break;
                default:
                        break;
                }
                break;
        case decl_t:
                switch (t->kind.decl) {
                case VarK:
                        break;
                case ArrayK:
                        break;
                case FunK:
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
        init_symbol_table();
        traverse(tree, update_symtab, NULL);
}
