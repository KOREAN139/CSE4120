%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"

int Error = FALSE;
int yyerror(char *);
static int yylex(void);
%}

%token IF ELSE INT RETURN VOID WHILE
%token ID NUM 
%token ASSIGN EQ NEQ LT LTE GT GTE PLUS MINUS TIMES OVER LPAREN RPAREN
%token LBRAC RBRAC LCURLY RCURLY COMMA SEMI
%token ENDOFFILE ERROR

/* left-associative, +,- has lower precedence than *,/ */
%left PLUS MINUS
%left TIMES OVER

%%

program         : decl_list
                ;

decl_list       : decl_list decl
                | decl
                ;

decl            : var_decl
                | fun_decl
                ;

var_decl        : type ID SEMI
                | type ID LBRAC NUM RBRAC SEMI
                ;

type            : INT
                | VOID
                ;

fun_decl        : type ID LPAREN params RPAREN comp_stmt
                ;

params          : param_list
                | VOID
                ;

param_list      : param_list COMMA param
                | param
                ;

param           : type ID
                | type ID LBRAC RBRAC
                ;

comp_stmt       : LCURLY local_decl stmt_list RCURLY
                ;

local_decl      : local_decl var_decl
                | /* empty */
                ;

stmt_list       : stmt_list stmt
                | /* empty */
                ;

stmt            : expr_stmt
                | comp_stmt
                | select_stmt
                | iter_stmt
                | return_stmt
                ;

expr_stmt       : expr SEMI
                | SEMI
                ;

select_stmt     : IF LPAREN expr RPAREN stmt
                | IF LPAREN expr RPAREN stmt ELSE stmt
                ;

iter_stmt       : WHILE LPAREN expr RPAREN stmt
                ;

return_stmt     : RETURN SEMI
                | RETURN expr SEMI
                ;

expr            : var ASSIGN expr
                | simple_expr
                ;

var             : ID
                | ID LBRAC expr RBRAC
                ;

simple_expr     : add_expr relop add_expr
                | add_expr
                ;

relop           : GTE
                | GT
                | LTE
                | LT
                | EQ
                | NEQ
                ;

add_expr        : add_expr addop term
                | term
                ;

addop           : PLUS
                | MINUS
                ;

term            : term mulop factor
                | factor
                ;

mulop           : TIMES
                | OVER
                ;

factor          : LPAREN expr RPAREN
                | var
                | call
                | NUM
                ;

call            : ID LPAREN args RPAREN
                ;

args            : arg_list
                | /* empty */
                ;

arg_list        : arg_list COMMA expr
                | expr
                ;

%%

int yyerror(char *message)
{
        fprintf(listing, "Syntax error at line %d: %s\n", lineno, message);
        fprintf(listing, "Current token: ");
        printToken(yychar, tokenString);
        Error = TRUE;
        return 0;
}

static int yylex(void)
{
        return getToken();
}

void parse(void)
{
        yyparse();
}
