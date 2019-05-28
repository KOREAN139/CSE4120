#include "globals.h"
#include "symtab.h"

/* symbol table implemented with hash table */
static symtab_t symtab_head, symtab_curr;
static int scope_level;
static char const *type_name[] = {
        "void", "int", "array"
};
static char const *symbol_type_name[] = {
        "var", "func", "param"
};

#define INCREASE_LEVEL (scope_level += 1)
#define DECREASE_LEVEL (scope_level -= 1)

/*
 * Function hash hashes given key string
 * returns hashed value
 * ret = sig(0 to n-1) (key[i] * 139^(n-i-1)) mod 211
 */
static int hash(char *key)
{
        int i, ret;
        for (i = ret = 1; *key; ++i) {
                ret *= POWER;
                ret += *key;
        }
        return ret % BASE;
}

/*
 * Function create_symbol_table creates new symbol table for
 * new scope
 */
static symtab_t create_symbol_table()
{
        symtab_t new = malloc(sizeof(struct _symtab_t));
        new->scope = scope_level;
        new->hash_table = malloc(sizeof(bucket_t) * BUCKET_SIZE);
        new->parent = NULL;
        new->child = NULL;
        new->sibling = NULL;
        return new;
}

void init_symbol_table()
{
        scope_level = 0;
        symtab_head = create_symbol_table();
        symtab_curr = symtab_head;
}

void child_symbol_table()
{
        INCREASE_LEVEL;
        symtab_t child = create_symbol_table();
        child->parent = symtab_curr;
        if (!symtab_curr->child) {
                symtab_curr->child = child;
        } else {
                symtab_t symtab_ptr = symtab_curr->child;
                while (symtab_ptr->sibling)
                        symtab_ptr = symtab_ptr->sibling;
                symtab_ptr->sibling = child;
        }
        symtab_curr = child;
}

void parent_symbol_table()
{
        DECREASE_LEVEL;
        symtab_curr = symtab_curr->parent;
}

void insert_symbol(char *name, type_t type, symbol_type_t symbol_type,
                int array_size, int lineno, int memloc)
{
        int index = hash(name);
        bucket_t *hash_table = symtab_curr->hash_table;
        bucket_t curr = hash_table[index];
        
        line_t line_info = malloc(sizeof(struct _line_t));
        line_info->lineno = lineno;
        line_info->next = NULL;

        while (curr && strcmp(name, curr->name))
                curr = curr->next;

        if (!curr) {
                curr = malloc(sizeof(struct _bucket_t));
                curr->name = name;
                curr->memloc = memloc;
                curr->lines = line_info;

                curr->next = hash_table[index];
                hash_table[index] = curr;
        } else {
                line_t t = curr->lines;

                while (t->next)
                        t = t->next;

                t->next = line_info;
        }
}

int lookup_symbol(char *name)
{
        int index = hash(name);
        symtab_t tab_ptr = symtab_curr;
        bucket_t *hash_table, curr;

        curr = NULL;

        while (tab_ptr && !curr) {
                hash_table = symtab_curr->hash_table;
                curr = hash_table[index];

                while (curr && strcmp(name, curr->name))
                        curr = curr->next;

                tab_ptr = tab_ptr->parent;
        }

        return curr ? curr->memloc : -1;
}

void print_symbol_table(FILE *listing)
{
}
