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
#include "tiny.tab.h"
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

/* Yacc/Bison uses its own token_t */
typedef int token_t;

/* source code text file */
extern FILE *source;
/* listing output text file */
extern FILE *listing;
/* code text file for TM simulator */
extern FILE *code;

/* source line number for listing */
extern int lineno;

/* specify node's kind */
typedef enum {
        StmtK, ExprK, DeclK
} node_kind_t;
/* specify statement's kind */
typedef enum {
        IfK, ReturnK, WhileK, CompK
} stmt_t;
/* specify expression's kind */
typedef enum {
        OpK, ConstK, IdK, TypeK, FunCallK, ArrSubK
} expr_t;
/* specify declaration's kind*/
typedef enum {
        VarK, ArrayK, FunK
} decl_t;
/* specify expression's type */
typedef enum {
        Void, Integer
} type_t;

#define MAXCHILDREN 4
/* structure of node in syntax tree */
typedef struct _node_t {
        struct _node_t *child[MAXCHILDREN];
        struct _node_t *sibling;
        int lineno;
        node_kind_t nodekind;
        union {
                stmt_t stmt;
                expr_t expr;
                decl_t decl;
        } kind;
        union {
                token_t op;
                int val;
                char *name;
        } attr;
        type_t type;
} node_t;

/* 
 * TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error; 

#endif
