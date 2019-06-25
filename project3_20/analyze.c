#include "analyze.h"
#include "globals.h"
#include "symtab.h"

#include <string.h>

extern int Error;

static int scope_level;
static int func_loc;
static int func_comp;

static node_t *func_ptr;

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
        bucket_t bucket_ptr;
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
                        _build_symbol_table(t->sibling);
                        break;
                case ReturnK:
                        t->def_ptr = func_ptr;
                        func_ptr->def_ptr = t;
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
                        if (!(bucket_ptr = lookup_symbol(t->attr.name))) {
                                // current variable is not visible in scope
                                Error = TRUE;
                                printf("Error in line %d : %s not exist\n",
                                                t->lineno, t->attr.name);
                                break;
                        }
                        add_symbol_line(t->attr.name, t->lineno);
                        t->type = bucket_ptr->type;
                        t->def_ptr = bucket_ptr->def_ptr;
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
                        if (insert_symbol(t, symbol_type, is_array,
                                                array_size, location)) {
                                Error = TRUE;
                                print_error_msg(t->child[1]->attr.name,
                                                t->lineno);
                                return;
                        }
                        t->child[1]->def_ptr = t;

                        update_memory_location(location);
                        _build_symbol_table(t->sibling);
                        break;
                case FunK:
                        symbol_type = Func;
                        if (insert_symbol(t, symbol_type, is_array,
                                                array_size, func_loc)) {
                                Error = TRUE;
                                print_error_msg(t->child[1]->attr.name,
                                                t->lineno);
                                return;
                        }
                        t->child[1]->def_ptr = t;

                        func_ptr = t;

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
                                if (insert_symbol(node_ptr, symbol_type,
                                                node_ptr->kind.decl == ArrayK,
                                                array_size, location)) {
                                        Error = TRUE;
                                        print_error_msg(t->child[1]->attr.name,
                                                        t->lineno);
                                        return;
                                }
                                node_ptr->child[1]->def_ptr = node_ptr;

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

static void _semantic_check(node_t *t)
{
        if (!t)
                return;

        int i;

        for (i = 0; i < MAXCHILDREN; i++)
                _semantic_check(t->child[i]);
        _semantic_check(t->sibling);

        if (Error)
                return;

        type_t t1, t2;
        node_t *node_ptr;
        node_t *param, *arg;

        switch (t->nodekind) {
        case StmtK:
                switch (t->kind.stmt) {
                case IfK:
                        t1 = t->child[0]->type;
                        if (t1 != Integer) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                puts("Condition statement must be INTEGER");
                                break;
                        }
                        break;
                case ReturnK:
                        node_ptr = t->def_ptr;
                        if (node_ptr->type == Void) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                printf("Void function should not");
                                puts(" have return statement");
                                break;
                        } else if (!t->child[0]) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                printf("Int function should have ");
                                puts("integer return value");
                                break;
                        } else if (t->child[0]->type != Integer) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                puts("Int function should return integer");
                                break;
                        }
                        break;
                case WhileK:
                        t1 = t->child[0]->type;
                        if (t1 != Integer) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                puts("Condition statement must be INTEGER");
                                break;
                        }
                        break;
                case CompK:
                        break;
                default:
                        break;
                }
                break;
        case ExprK:
                switch (t->kind.expr) {
                case OpK:
                        t1 = t->child[0]->type;
                        t2 = t->child[1]->type;

                        if (t1 != t2) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                puts("Operation between different types");
                                break;
                        }

                        if (t1 != Integer || t2 != Integer) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                puts("Operation with non-integer variable");
                                break;
                        }

                        t->type = t1;
                        break;
                case ConstK:
                        t->type = Integer;
                        break;
                case IdK:
                        t->type = t->def_ptr->type;
                        break;
                case FunCallK:
                        node_ptr = t->child[0]->def_ptr;
                        if (node_ptr->kind.decl != FunK) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                puts("Using non-function as function");
                                break;
                        }

                        param = node_ptr->child[2];
                        arg = t->child[1];

                        while (param && arg) {
                                if (param->type != arg->type) {
                                        Error = TRUE;
                                        break;
                                }

                                param = param->sibling;
                                arg = arg->sibling;
                        }

                        if (param || arg) {
                                printf("Error in line %d: ", t->lineno);
                                if (Error) {
                                        puts("Argument with wrong type");
                                } else if (param) {
                                        puts("Not enough arguments");
                                } else {
                                        puts("Too many arguments");
                                }
                                Error = TRUE;
                        }

                        t->type = t->child[0]->type;
                        break;
                case ArrSubK:
                        node_ptr = t->child[0]->def_ptr;
                        if (node_ptr->kind.decl != ArrayK) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                puts("Using non-array variable as array");
                                break;
                        }

                        t1 = t->child[1]->type;
                        if (t1 != Integer) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                puts("Array index must be INTEGER");
                                break;
                        }

                        t->type = t1;
                        break;
                default:
                        break;
                }
                break;
        case DeclK:
                switch (t->kind.decl) {
                case ArrayK:
                case VarK:
                        t1 = t->child[0]->type;
                        if (t1 == Void) {
                                Error = TRUE;
                                printf("Error in line %d: ", t->lineno);
                                puts("Variable cannot be VOID");
                                break;
                        }
                        break;
                case FunK:
                        t1 = t->child[0]->type;
                        if (!strcmp("main", t->child[1]->attr.name)) {
                                if (t1 != Void) {
                                        Error = TRUE;
                                        printf("Error in line %d: ", t->lineno);
                                        puts("main function must be VOID");
                                        break;
                                }
                                if (t->child[2]) {
                                        Error = TRUE;
                                        printf("Error in line %d: ", t->lineno);
                                        printf("main function must have no");
                                        puts("parameter");
                                        break;
                                }
                                if (t->sibling) {
                                        Error = TRUE;
                                        printf("Error in line %d: ", t->lineno);
                                        printf("main function must be located");
                                        puts("at the end");
                                        break;
                                }
                                break;
                        } else {
                                if (!t->def_ptr && t->type == Integer) {
                                        Error = TRUE;
                                        printf("Error in line %d: ", t->lineno);
                                        puts("Omitted return statement");
                                        break;
                                }
                        }

                        break;
                default:
                        break;
                }
                break;
        default:
                break;
        }
}

void semantic_check(node_t *tree)
{
        _semantic_check(tree);
}
