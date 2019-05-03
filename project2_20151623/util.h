#ifndef _UTIL_H_
#define _UTIL_H_

#include "globals.h"
#include <stdlib.h>
#include <string.h>

/*
 * Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken(TokenType, const char *);

/*
 * Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *);

/*
 * Function copyValue calculates value of given string
 * and returns its value
 */
int copyValue(char *);

/*
 * newStmtNode creates new statement node, initialize with given kind,
 * then return initialized node
 */
TreeNode *newStmtNode(StmtKind);
/*
 * newExprNode creates new expression node, initialize with given kind,
 * then return initialized node
 */
TreeNode *newExprNode(ExprKind);
/*
 * newDeclNode creates new declaration node, initialize with given kind,
 * then return initialized node
 */
TreeNode *newDeclNode(DeclKind);

/*
 * procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *);
#endif
