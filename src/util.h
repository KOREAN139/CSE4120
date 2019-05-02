#ifndef _UTIL_H_
#define _UTIL_H_

#include "globals.h"
#include <stdlib.h>

/* 
 * Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken(TokenType, const char *);

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
#endif
