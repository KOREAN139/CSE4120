#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 6

typedef enum {
        /* book-keeping tokens */
        ENDFILE, ERROR,
        /* reserved words */
        IF, ELSE, INT, RETURN, VOID, WHILE,
        /* multicharacter tokens */
        ID, NUM,
        /* special symbols */
        ASSIGN, EQ, NEQ, LT, LTE, GT, GTE, PLUS, MINUS, TIMES, OVER,
        LPAREN, RPAREN, LBRAC, RBRAC, LCURLY, RCURLY, COMMA, SEMI
} TokenType;

/* source code text file */
extern FILE *source;
/* listing output text file */
extern FILE *listing;
/* code text file for TM simulator */
extern FILE *code;

/* source line number for listing */
extern int lineno;

/* 
 * TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

#endif
