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
