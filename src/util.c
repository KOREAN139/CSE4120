#include "globals.h"
#include "util.h"

static char const *token_name[] = {
        "IF", "ELSE", "INT", "RETURN", "VOID", "WHILE",
        "ID", "NUM", "=", "==", "!=", "<", "<=", ">",
        ">=", "+", "-", "*", "/", "(", ")", "[", "]",
        "{", "}", ",", ";", "ERROR"
};

static char const *stmt_name[] = {
        "If", "Return", "While", "Compound"
};
static char const *decl_name[] = {
        "Variable", "Array", "Function"
};
static char const *type_name[] = {
        "void", "int"
};

static int indent_level = 0;

/* 
 * Procedure print_token prints a token
 * and its lexeme to the listing file
 */
void print_token(token_t token, const char *token_string)
{ 
        switch (token) { 
        case IF:
        case ELSE:
        case INT:
        case RETURN:
        case VOID:
        case WHILE:
        case ASSIGN:
        case EQ:
        case NEQ:
        case LT:
        case LTE:
        case GT:
        case GTE:
        case LPAREN:
        case RPAREN:
        case LBRAC:
        case RBRAC:
        case LCURLY:
        case RCURLY:
        case COMMA:
        case SEMI:
        case PLUS:
        case MINUS:
        case TIMES:
        case OVER:
        case NUM:
        case ID:
        case ERROR:
                fprintf(listing, "\t\t%s\t\t%s\n",
                        token_name[token-IF], token_string);
                break;
        case ENDFILE:
                fprintf(listing, "\t\t%s\t\t%s\n",
                        "EOF", token_string);
                break;
        default: /* should never happen */
                fprintf(listing, "Unknown token: %d\n", token);
        }
}

/*
 * Function copy_string allocates and makes a new
 * copy of an existing string
 */
char *copy_string(char *s)
{
        if (!s)
                return NULL;
        int n = strlen(s) + 1;
        char *t = malloc(n);
        if (!t)
                fprintf(listing, "Out of memory error at line %d\n", lineno);
        else
                strcpy(t,s);
        return t;
}

/*
 * Function copy_value calculates value of given string
 * and returns its value
 */
int copy_value(char *s)
{
        return atoi(s);
}

/*
 * new_stmt_node creates new statement node, initialize with given kind,
 * then return initialized node
 */
node_t *new_stmt_node(stmt_t kind)
{
        /*
         * The conversion from void pointer to any other pointer type is
         * guaranteed by the C programming language
         */
        node_t *t = malloc(sizeof(node_t));
        int i;
        if (!t) {
                fprintf(listing, "Out of memory error at line %d\n", lineno);
        } else {
                for (i = 0; i < MAXCHILDREN; i++)
                        t->child[i] = NULL;
                t->sibling = NULL;
                t->nodekind = StmtK;
                t->kind.stmt = kind;
                t->lineno = lineno;
        }
        return t;
}

/*
 * new_expr_node creates new expression node, initialize with given kind,
 * then return initialized node
 */
node_t *new_expr_node(expr_t kind)
{
        node_t *t = malloc(sizeof(node_t));
        int i;
        if (!t) {
                fprintf(listing, "Out of memory error at line %d\n", lineno);
        } else {
                for (i = 0; i < MAXCHILDREN; i++)
                        t->child[i] = NULL;
                t->sibling = NULL;
                t->nodekind = ExprK;
                t->kind.stmt = kind;
                t->lineno = lineno;
        }
        return t;
}

/*
 * new_decl_node creates new declaration node, initialize with given kind,
 * then return initialized node
 */
node_t *new_decl_node(decl_t kind)
{
        node_t *t = malloc(sizeof(node_t));
        int i;
        if (!t) {
                fprintf(listing, "Out of memory error at line %d\n", lineno);
        } else {
                for (i = 0; i < MAXCHILDREN; i++)
                        t->child[i] = NULL;
                t->sibling = NULL;
                t->nodekind = DeclK;
                t->kind.stmt = kind;
                t->lineno = lineno;
        }
        return t;
}

/* print_spaces indents by printing spaces */
static void print_spaces()
{
        int i;
        for (i = 0; i < indent_level; i++)
                fprintf(listing, "  ");
}

/*
 * procedure print_tree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void print_tree(node_t *curr)
{
        int i;
        indent_level += 1;
        while (curr) {
                print_spaces();
                if (curr->nodekind == DeclK) {
                        switch (curr->kind.decl) {
                        case VarK:
                        case ArrayK:
                        case FunK:
                                fprintf(listing, "%s Declaration\n",
                                                decl_name[curr->kind.decl]);
                                break;
                        default:
                                fprintf(listing, "Unknown DeclNode kind\n");
                                break;
                        }
                } else if (curr->nodekind == StmtK) {
                        switch (curr->kind.stmt) {
                        case IfK:
                        case WhileK:
                        case CompK:
                        case ReturnK:
                                fprintf(listing, "%s\n",
                                                stmt_name[curr->kind.stmt]);
                                break;
                        default:
                                fprintf(listing, "Unknown StmtNode kind\n");
                                break;
                        }
                } else if (curr->nodekind == ExprK) {
                        switch (curr->kind.expr) {
                        case OpK:
                                fprintf(listing, "Op: %s\n",
                                                token_name[curr->attr.op-IF]);
                                break;
                        case ConstK:
                                fprintf(listing, "Const: %d\n",
                                                curr->attr.val);
                                break;
                        case IdK:
                                fprintf(listing, "ID: %s\n",
                                                curr->attr.name);
                                break;
                        case TypeK:
                                fprintf(listing, "Type: %s\n",
                                                type_name[curr->type]);
                                break;
                        case FunCallK:
                                fprintf(listing, "Call\n");
                                break;
                        case ArrSubK:
                                fprintf(listing, "Array Subscript\n");
                                break;
                        default:
                                fprintf(listing, "Unknown ExprNode kind\n");
                                break;
                        }
                } else {
                        fprintf(listing, "Unknown node kind\n");
                }
                for (i = 0; i < MAXCHILDREN; i++)
                        print_tree(curr->child[i]);
                curr = curr->sibling;
        }
        indent_level -= 1;
}
