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
        int ret = 0;
        while (*key) {
                ret *= POWER;
                ret += *key++;
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
        new->memory_location = 0;
        new->hash_table = calloc(BUCKET_SIZE, sizeof(bucket_t));
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

int insert_symbol(char *name, type_t type, symbol_type_t symbol_type,
                int is_array, int array_size, int lineno, int memloc)
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
                curr->type = type;
                curr->symbol_type = symbol_type;
                curr->is_array = is_array;
                curr->array_size = array_size;
                curr->memloc = memloc;
                curr->lines = line_info;

                curr->next = hash_table[index];
                hash_table[index] = curr;
                return 0;
        } else {
                return curr->lines->lineno;
        }
}

void add_symbol_line(char *name, int lineno)
{
        int index = hash(name);
        symtab_t symtab_ptr = symtab_curr;
        bucket_t curr = NULL;

        line_t line_info = malloc(sizeof(struct _line_t));
        line_info->lineno = lineno;
        line_info->next = NULL;

        while (!curr && symtab_ptr) {
                curr = symtab_ptr->hash_table[index];
                while (curr && strcmp(name, curr->name))
                        curr = curr->next;
                symtab_ptr = symtab_ptr->parent;
        }

        if (curr) {
                line_t t = curr->lines;

                while (t->next)
                        t = t->next;

                t->next = line_info;
        }
}

bucket_t lookup_symbol(char *name)
{
        int index = hash(name);
        symtab_t tab_ptr = symtab_curr;
        bucket_t *hash_table, curr;

        curr = NULL;

        while (tab_ptr && !curr) {
                curr = tab_ptr->hash_table[index];

                while (curr && strcmp(name, curr->name))
                        curr = curr->next;

                tab_ptr = tab_ptr->parent;
        }

        return curr;
}

static void _print_symbol_table(FILE *listing, symtab_t t)
{
        int i;
        line_t line_ptr;
        bucket_t bucket_ptr;
        symtab_t symtab_ptr = t->child;
        while (symtab_ptr) {
                _print_symbol_table(listing, symtab_ptr);
                symtab_ptr = symtab_ptr->sibling;
        }
        fprintf(listing, "Name\tScope\tLoc\tV/P/F\tArray?\t%s",
                        "ArrSize\tType\tLine Numbers\n");
        for (i = 0; i < BUCKET_SIZE; i++) {
                bucket_ptr = t->hash_table[i];
                if (!bucket_ptr)
                        continue;
                fprintf(listing, "%s\t%d\t%d\t%s\t%s\t",
                                bucket_ptr->name, t->scope,
                                bucket_ptr->memloc,
                                symbol_type_name[bucket_ptr->symbol_type],
                                bucket_ptr->is_array ? "Yes" : "No");
                if (bucket_ptr->is_array)
                        fprintf(listing, "%d\t", bucket_ptr->array_size);
                else
                        fprintf(listing, "-\t");
                fprintf(listing, "%s\t", type_name[bucket_ptr->type]);
                line_ptr = bucket_ptr->lines;
                while (line_ptr) {
                        fprintf(listing, "%d\t", line_ptr->lineno);
                        line_ptr = line_ptr->next;
                }
                puts("");
        }
        puts("");
}

int get_memory_location()
{
        return symtab_curr->memory_location;
}

void update_memory_location(int location)
{
        symtab_curr->memory_location = location;
}

void print_symbol_table(FILE *listing)
{
        _print_symbol_table(listing, symtab_head);
}
