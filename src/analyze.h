#ifndef _ANALYZE_H_
#define _ANALYZE_H_

#include "globals.h"

/*
 * build_symbol_table builds symbol table based on given syntax tree
 * by preorder traversal
 */
void build_symbol_table(node_t *);

#endif
