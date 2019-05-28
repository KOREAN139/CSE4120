#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <string.h>
#include <stdlib.h>
#include "globals.h"

#define POWER 139
#define BASE 211
#define BUCKET_SIZE BASE

/* specify symbol's type */
typedef enum {
        Var, Func, Param
} symbol_type_t;

/* structure for tracing variable's referenced location */
typedef struct _line_t {
        int lineno;
        struct _line_t *next;
} *line_t;

/* structure for bucket list which contains scope's information */
typedef struct _bucket_t {
        char *name;
        line_t lines;
        type_t type;
        symbol_type_t symbol_type;
        int is_array;
        int array_size;
        int memloc;
        struct _bucket_t *next;
} *bucket_t;

/* structure for symbol table */
typedef struct _symtab_t {
        int scope;
        int memory_location;
        bucket_t *hash_table;
        struct _symtab_t *parent;
        struct _symtab_t *child;
        struct _symtab_t *sibling;
} *symtab_t;

/* init_symbol_table initialize symbol table */
void init_symbol_table();

/*
 * child_symbol_table creates new symbol table & links as child
 * to current symbol table
 * move current symbol table pointer to child
 */
void child_symbol_table();

/* parent_symbol_table moves current symbol table pointer to parent */
void parent_symbol_table();

/* get_memory_location returns memory location of current symbol table */
int get_memory_location();

/* update_memory_location updates memory location of current symbol table */
void update_memory_location(int);

/*
 * insert_symbol inserts variable into symbol table
 */
int insert_symbol(char *, type_t, symbol_type_t, int, int, int, int);

/*
 * add_symbol_line add line number to given variable
 */
void add_symbol_line(char *, int);

/*
 * lookup_symbol finds given variable's memory location
 */
bucket_t lookup_symbol(char *);

/*
 * print symbol table with formatting
 */
void print_symbol_table(FILE *);

#endif
