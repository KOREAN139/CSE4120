#include "globals.h"
#include "util.h"

static char const *tokenName[] = {
        "IF", "ELSE", "INT", "RETURN", "VOID", "WHILE",
        "ID", "NUM", "=", "==", "!=", "<", "<=", ">",
        ">=", "+", "-", "*", "/", "(", ")", "[", "]",
        "{", "}", ",", ";", "EOF", "ERROR"
};

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
        case ENDOFFILE:
        case NUM:
        case ID:
        case ERROR:
                fprintf(listing, "\t\t%s\t\t%s\n",
                        tokenName[token-IF], tokenString);
                break;
        default: /* should never happen */
                fprintf(listing, "Unknown token: %d\n", token);
        }
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
}
