#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <string.h>
#include <stdlib.h>

#define POWER 139
#define BASE 211
#define BUCKET_SIZE BASE

/* structure for tracing variable's referenced location */
typedef struct _line_t {
        int lineno;
        struct _line_t *next;
} *line_t;

/* structure for bucket list which contains scope's information */
typedef struct _bucket_t {
        char *name;
        line_t lines;
        int memloc;
        struct _bucket_t *next;
} *bucket_t;

/* structure for symbol table */
typedef struct _symtab_t {
        int scope;
        bucket_t *hash_table;
        struct _symtab_t *parent;
        struct _symtab_t *child;
        struct _symtab_t *sibling;
} *symtab_t;

/* init_symbol_table initialize symbol table */
void init_symbol_table();

/*
 * insert_symbol inserts current variable into symbol table
 * memory location is inserted only the first time
 * otherwise, it's ignored
 */
void insert_symbol(char *, int, int);

/*
 * lookup_symbol finds given variable's memory location
 */
int lookup_symbol(char *);

/*
 * print symbol table with formatting
 */
void print_symbol_table(FILE *);

#endif
