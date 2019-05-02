#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*
 * Yacc/Bison generates internally its own values
 * for the tokens. Other files can access these values
 * by including the tab.h file generated using the
 * Yacc/Bison option -d ("generate header")
 *
 * The YYPARSER flag prevents inclusion of the tab.h
 * into the Yacc/Bison output itself
 */
#ifndef YYPARSER
#include "y.tab.h"
/*
 * ENDFILE is implicitly defined by Yacc/Bison,
 * and not included in the tab.h file
 */
#define ENDFILE 0
#endif

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

/* Error = TRUE prevents further passes if an error occurs */
extern int Error; 

#endif
