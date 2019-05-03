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

/* Yacc/Bison uses its own TokenType */
typedef int TokenType;

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
} NodeKind;
/* specify statement's kind */
typedef enum {
        IfK, ReturnK, WhileK, CompK
} StmtKind;
/* specify expression's kind */
typedef enum {
        OpK, ConstK, IdK, AssignK, FunCallK
} ExprKind;
/* specify declaration's kind*/
typedef enum {
        TypeK, VarK, ArrayK, FunK
} DeclKind;
/* specify expression's type */
typedef enum {
        Void, Integer
} ExprType;

#define MAXCHILDREN 3
/* structure of node in syntax tree */
typedef struct _TreeNode {
        struct _TreeNode *child[MAXCHILDREN];
        struct _TreeNode *sibling;
        int lineno;
        NodeKind nodekind;
        union {
                StmtKind stmt;
                ExprKind expr;
                DeclKind decl;
        } kind;
        union {
                TokenType op;
                int val;
                char *name;
        } attr;
        int val;
        ExprType type;
} TreeNode;

/* 
 * TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error; 

#endif
