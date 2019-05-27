%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"

#define YYSTYPE node_t *
int Error = FALSE;
static char *saved_name;
static char *saved_func;
static int saved_lineno;
static int saved_value;
static int saved_op;
static node_t *syntax_tree;

int yyerror(char *);
static int yylex(void);
%}

%token IF ELSE INT RETURN VOID WHILE
%token ID NUM 
%token ASSIGN EQ NEQ LT LTE GT GTE PLUS MINUS TIMES OVER LPAREN RPAREN
%token LBRAC RBRAC LCURLY RCURLY COMMA SEMI
%token ERROR

/* left-associative, +,- has lower precedence than *,/ */
%left PLUS MINUS
%left TIMES OVER

%%

program         : decl_list
                        {
                                syntax_tree = $1;
                        }
                ;

decl_list       : decl_list decl
                        {
                                YYSTYPE t = $1;
                                if (t) {
                                        while (t->sibling)
                                                t = t->sibling;
                                        t->sibling = $2;
                                        $$ = $1;
                                } else {
                                        $$ = $2;
                                }
                        }
                | decl
                        {
                                $$ = $1;
                        }
                ;

decl            : var_decl
                        {
                                $$ = $1;
                        }
                | fun_decl
                        {
                                $$ = $1;
                        }
                ;

var_decl        : type ID 
                        {
                                saved_name = copy_string(id_string);
                                saved_lineno = lineno;
                        }
                  SEMI
                        {
                                $$ = new_decl_node(VarK);
                                $$->lineno = saved_lineno;
                                $$->child[0] = $1;
                                $$->child[1] = new_expr_node(IdK);
                                $$->child[1]->attr.name = saved_name;
                                $$->type = $1->type;
                        }
                | type ID
                        {
                                saved_name = copy_string(id_string);
                                saved_lineno = lineno;
                        }
                  LBRAC NUM
                        {
                                saved_value = copy_value(token_string);
                        }
                  RBRAC SEMI
                        {
                                $$ = new_decl_node(ArrayK);
                                $$->lineno = saved_lineno;
                                $$->child[0] = $1;
                                $$->child[1] = new_expr_node(IdK);
                                $$->child[1]->attr.name = saved_name;
                                $$->child[2] = new_expr_node(ConstK);
                                $$->child[2]->attr.val = saved_value;
                                $$->type = $1->type;
                        }
                ;

type            : INT
                        {
                                $$ = new_expr_node(TypeK);
                                $$->type = Integer;
                        }
                | VOID
                        {
                                $$ = new_expr_node(TypeK);
                                $$->type = Void;
                        }
                ;

fun_decl        : type ID
                        {
                                saved_func = copy_string(id_string);
                                saved_lineno = lineno;
                        }
                  LPAREN params RPAREN comp_stmt
                        {
                                $$ = new_decl_node(FunK);
                                $$->lineno = saved_lineno;
                                $$->child[0] = $1;
                                $$->child[1] = new_expr_node(IdK);
                                $$->child[1]->attr.name = saved_func;
                                $$->child[2] = $5;
                                $$->child[3] = $7;
                                $$->type = $1->type;
                        }
                ;

params          : param_list
                        {
                                $$ = $1;
                        }
                | VOID
                        {
                                $$ = NULL;
                        }
                ;

param_list      : param_list COMMA param
                        {
                                YYSTYPE t = $1;
                                if (t) {
                                        while (t->sibling)
                                                t = t->sibling;
                                        t->sibling = $3;
                                        $$ = $1;
                                } else {
                                        $$ = $3;
                                }
                        }
                | param
                        {
                                $$ = $1;
                        }
                ;

param           : type ID
                        {
                                $$ = new_decl_node(VarK);
                                $$->attr.name = copy_string(id_string);
                                $$->lineno = lineno;
                                $$->child[0] = $1;
                                $$->child[1] = new_expr_node(IdK);
                                $$->child[1]->attr.name =
                                copy_string(id_string);
                                $$->type = $1->type;
                        }
                | type ID
                        {
                                saved_name = copy_string(id_string);
                                saved_lineno = lineno;
                        }
                  LBRAC RBRAC
                        {
                                $$ = new_decl_node(ArrayK);
                                $$->lineno = saved_lineno;
                                $$->child[0] = $1;
                                $$->child[1] = new_expr_node(IdK);
                                $$->child[1]->attr.name = saved_name;
                                $$->child[2] = new_expr_node(ConstK);
                                $$->child[2]->attr.val = 0;
                                $$->type = $1->type;
                        }
                ;

comp_stmt       : LCURLY local_decl stmt_list RCURLY
                        {
                                $$ = new_stmt_node(CompK);
                                $$->child[0] = $2;
                                $$->child[1] = $3;
                        }
                ;

local_decl      : local_decl var_decl
                        {
                                YYSTYPE t = $1;
                                if (t) {
                                        while (t->sibling)
                                                t = t->sibling;
                                        t->sibling = $2;
                                        $$ = $1;
                                } else {
                                        $$ = $2;
                                }
                        }
                | /* empty */
                        {
                                $$ = NULL;
                        }
                ;

stmt_list       : stmt_list stmt
                        {
                                YYSTYPE t = $1;
                                if (t) {
                                        while (t->sibling)
                                                t = t->sibling;
                                        t->sibling = $2;
                                        $$ = $1;
                                } else {
                                        $$ = $2;
                                }
                        }
                | /* empty */
                        {
                                $$ = NULL;
                        }
                ;

stmt            : expr_stmt
                        {
                                $$ = $1;
                        }
                | comp_stmt
                        {
                                $$ = $1;
                        }
                | select_stmt
                        {
                                $$ = $1;
                        }
                | iter_stmt
                        {
                                $$ = $1;
                        }
                | return_stmt
                        {
                                $$ = $1;
                        }
                ;

expr_stmt       : expr SEMI
                        {
                                $$ = $1;
                        }
                | SEMI
                        {
                                $$ = NULL;
                        }
                ;

select_stmt     : IF LPAREN expr RPAREN stmt
                        {
                                $$ = new_stmt_node(IfK);
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                        }
                | IF LPAREN expr RPAREN stmt ELSE stmt
                        {
                                $$ = new_stmt_node(IfK);
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                                $$->child[2] = $7;
                        }
                ;

iter_stmt       : WHILE LPAREN expr RPAREN stmt
                        {
                                $$ = new_stmt_node(WhileK);
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                        }
                ;

return_stmt     : RETURN SEMI
                        {
                                $$ = new_stmt_node(ReturnK);
                        }
                | RETURN expr SEMI
                        {
                                $$ = new_stmt_node(ReturnK);
                                $$->child[0] = $2;
                        }
                ;

expr            : var ASSIGN expr
                        {
                                $$ = new_expr_node(OpK);
                                $$->attr.op = ASSIGN;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                        }
                | simple_expr
                        {
                                $$ = $1;
                        }
                ;

var             : ID
                        {
                                $$ = new_expr_node(IdK);
                                $$->attr.name = copy_string(id_string);
                                $$->lineno = lineno;
                        }
                | ID
                        {
                                saved_name = copy_string(id_string);
                                saved_lineno = lineno;
                        }
                  LBRAC expr RBRAC
                        {
                                $$ = new_expr_node(ArrSubK);
                                $$->lineno = saved_lineno;
                                $$->child[0] = new_expr_node(IdK);
                                $$->child[0]->attr.name = saved_name;
                                $$->child[1] = $4;
                        }
                ;

simple_expr     : add_expr relop add_expr
                        {
                                $$ = new_expr_node(OpK);
                                $$->attr.op = saved_op;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                        }
                | add_expr
                        {
                                $$ = $1;
                        }
                ;

relop           : GTE
                        {
                                saved_op = GTE;
                        }
                | GT
                        {
                                saved_op = GT;
                        }
                | LTE
                        {
                                saved_op = LTE;
                        }
                | LT
                        {
                                saved_op = LT;
                        }
                | EQ
                        {
                                saved_op = EQ;
                        }
                | NEQ
                        {
                                saved_op = NEQ;
                        }
                ;

add_expr        : add_expr addop term
                        {
                                $$ = new_expr_node(OpK);
                                $$->attr.op = saved_op;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                        }
                | term
                        {
                                $$ = $1;
                        }
                ;

addop           : PLUS
                        {
                                saved_op = PLUS;
                        }
                | MINUS
                        {
                                saved_op = MINUS;
                        }
                ;

term            : term mulop factor
                        {
                                $$ = new_expr_node(OpK);
                                $$->attr.op = saved_op;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                        }
                | factor
                        {
                                $$ = $1;
                        }
                ;

mulop           : TIMES
                        {
                                saved_op = TIMES;
                        }
                | OVER
                        {
                                saved_op = OVER;
                        }
                ;

factor          : LPAREN expr RPAREN
                        {
                                $$ = $2;
                        }
                | var
                        {
                                $$ = $1;
                        }
                | call
                        {
                                $$ = $1; 
                        }
                | NUM
                        {
                                $$ = new_expr_node(ConstK);
                                $$->attr.val = copy_value(token_string);
                        }
                ;

call            : ID
                        {
                                saved_name = copy_string(id_string);
                                saved_lineno = lineno;
                        }
                  LPAREN args RPAREN
                        {
                                $$ = new_expr_node(FunCallK);
                                $$->lineno = saved_lineno;
                                $$->child[0] = new_expr_node(IdK);
                                $$->child[0]->attr.name = saved_name;
                                $$->child[1] = $4;
                        }
                ;

args            : arg_list
                        {
                                $$ = $1;
                        }
                | /* empty */
                        {
                                $$ = NULL;
                        }
                ;

arg_list        : arg_list COMMA expr
                        {
                                YYSTYPE t = $1;
                                if (t) {
                                        while (t->sibling)
                                                t = t->sibling;
                                        t->sibling = $3;
                                        $$ = $1;
                                } else {
                                        $$ = $3;
                                }
                        }
                | expr
                        {
                                $$ = $1;
                        }
                ;

%%

int yyerror(char *message)
{
        fprintf(listing, "Syntax error at line %d: %s\n", lineno, message);
        fprintf(listing, "Current token: ");
        print_token(yychar, token_string);
        Error = TRUE;
        return 0;
}

static int yylex(void)
{
        return get_token();
}

node_t *parse(void)
{
        yyparse();
        return syntax_tree;
}
