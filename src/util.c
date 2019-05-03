#include "globals.h"
#include "util.h"

static char const *tokenName[] = {
        "IF", "ELSE", "INT", "RETURN", "VOID", "WHILE",
        "ID", "NUM", "=", "==", "!=", "<", "<=", ">",
        ">=", "+", "-", "*", "/", "(", ")", "[", "]",
        "{", "}", ",", ";", "ERROR"
};

static char const *stmtName[] = {
        "If", "Return", "While", "Compound"
};
static char const *declName[] = {
        "Variable", "Array", "Function"
};
static char const *typeName[] = {
        "void", "int"
};

static int indentLevel = 0;

/* 
 * Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char *tokenString)
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
                        tokenName[token-IF], tokenString);
                break;
        case ENDFILE:
                fprintf(listing, "\t\t%s\t\t%s\n",
                        "EOF", tokenString);
                break;
        default: /* should never happen */
                fprintf(listing, "Unknown token: %d\n", token);
        }
}

/*
 * Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s)
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
 * Function copyValue calculates value of given string
 * and returns its value
 */
int copyValue(char *s)
{
        return atoi(s);
}

/*
 * newStmtNode creates new statement node, initialize with given kind,
 * then return initialized node
 */
TreeNode *newStmtNode(StmtKind kind)
{
        /*
         * The conversion from void pointer to any other pointer type is
         * guaranteed by the C programming language
         */
        TreeNode *t = malloc(sizeof(TreeNode));
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
 * newExprNode creates new expression node, initialize with given kind,
 * then return initialized node
 */
TreeNode *newExprNode(ExprKind kind)
{
        TreeNode *t = malloc(sizeof(TreeNode));
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
 * newDeclNode creates new declaration node, initialize with given kind,
 * then return initialized node
 */
TreeNode *newDeclNode(DeclKind kind)
{
        TreeNode *t = malloc(sizeof(TreeNode));
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

/* printSpaces indents by printing spaces */
static void printSpaces()
{
        int i;
        for (i = 0; i < indentLevel; i++)
                fprintf(listing, "  ");
}

/*
 * procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *curr)
{
        int i;
        indentLevel += 1;
        while (curr) {
                printSpaces();
                if (curr->nodekind == DeclK) {
                        switch (curr->kind.decl) {
                        case VarK:
                        case ArrayK:
                        case FunK:
                                fprintf(listing, "%s Declaration\n",
                                                declName[curr->kind.decl]);
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
                                                stmtName[curr->kind.stmt]);
                                break;
                        default:
                                fprintf(listing, "Unknown StmtNode kind\n");
                                break;
                        }
                } else if (curr->nodekind == ExprK) {
                        switch (curr->kind.expr) {
                        case OpK:
                                fprintf(listing, "Op: %s\n",
                                                tokenName[curr->attr.op-IF]);
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
                                                typeName[curr->type]);
                                break;
                        case FunCallK:
                                fprintf(listing, "Call %s\n",
                                                curr->attr.name);
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
                        printTree(curr->child[i]);
                curr = curr->sibling;
        }
        indentLevel -= 1;
}
