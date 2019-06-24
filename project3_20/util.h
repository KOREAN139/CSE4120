#ifndef _UTIL_H_
#define _UTIL_H_

#include "globals.h"
#include <stdlib.h>
#include <string.h>

/*
 * Procedure print_token prints a token
 * and its lexeme to the listing file
 */
void print_token(token_t, const char *);

/*
 * Function copy_string allocates and makes a new
 * copy of an existing string
 */
char *copy_string(char *);

/*
 * Function copy_value calculates value of given string
 * and returns its value
 */
int copy_value(char *);

/*
 * new_stmt_node creates new statement node, initialize with given kind,
 * then return initialized node
 */
node_t *new_stmt_node(stmt_t);
/*
 * new_expr_node creates new expression node, initialize with given kind,
 * then return initialized node
 */
node_t *new_expr_node(expr_t);
/*
 * new_decl_node creates new declaration node, initialize with given kind,
 * then return initialized node
 */
node_t *new_decl_node(decl_t);

/*
 * procedure print_tree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void print_tree(node_t *);
#endif
