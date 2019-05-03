%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"

#define YYSTYPE TreeNode *
int Error = FALSE;
static char *savedName;
static int savedLineNo;
static int savedValue;
static int savedOp;
static TreeNode *syntaxTree;

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
                        {
                                syntaxTree = $1;
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
                                savedName = copyString(tokenString);
                                savedLineNo = lineno;
                        }
                  SEMI
                        {
                                $$ = newDeclNode(VarK);
                                $$->attr.name = savedName;
                                $$->lineno = savedLineNo;
                                $$->type = $1->type;
                        }
                | type ID
                        {
                                savedName = copyString(tokenString);
                                savedLineNo = lineno;
                        }
                  LBRAC NUM
                        {
                                savedValue = copyValue(tokenString);
                        }
                  RBRAC SEMI
                        {
                                $$ = newDeclNode(ArrayK);
                                $$->attr.name = savedName;
                                $$->lineno = savedLineNo;
                                $$->val = savedValue;
                                $$->type = $1->type;
                        }
                ;

type            : INT
                        {
                                $$ = newDeclNode(TypeK);
                                $$->lineno = lineno;
                                $$->type = Integer;
                        }
                | VOID
                        {
                                $$ = newDeclNode(TypeK);
                                $$->lineno = lineno;
                                $$->type = Void;
                        }
                ;

fun_decl        : type ID
                        {
                                savedName = copyString(tokenString);
                                savedLineNo = lineno;
                        }
                  LPAREN params RPAREN comp_stmt
                        {
                                $$ = newDeclNode(FunK);
                                $$->attr.name = savedName;
                                $$->lineno = savedLineNo;
                                $$->type = $1->type;
                                $$->child[0] = $5;
                                $$->child[0] = $7;
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
                                $$ = newDeclNode(VarK);
                                $$->attr.name = copyString(tokenString);
                                $$->lineno = lineno;
                                $$->type = $1->type;
                        }
                | type ID
                        {
                                savedName = copyString(tokenString);
                                savedLineNo = lineno;
                        }
                  LBRAC RBRAC
                        {
                                $$ = newDeclNode(ArrayK);
                                $$->attr.name = savedName;
                                $$->lineno = savedLineNo;
                                $$->type = $1->type;
                        }
                ;

comp_stmt       : LCURLY local_decl stmt_list RCURLY
                        {
                                $$ = newStmtNode(CompK);
                                $$->lineno = lineno;
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
                                $$ = newStmtNode(IfK);
                                $$->lineno = lineno;
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                        }
                | IF LPAREN expr RPAREN stmt ELSE stmt
                        {
                                $$ = newStmtNode(IfK);
                                $$->lineno = lineno;
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                                $$->child[2] = $7;
                        }
                ;

iter_stmt       : WHILE LPAREN expr RPAREN stmt
                        {
                                $$ = newStmtNode(WhileK);
                                $$->lineno = lineno;
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                        }
                ;

return_stmt     : RETURN SEMI
                        {
                                $$ = newStmtNode(ReturnK);
                                $$->lineno = lineno;
                        }
                | RETURN expr SEMI
                        {
                                $$ = newStmtNode(ReturnK);
                                $$->lineno = lineno;
                                $$->child[0] = $2;
                        }
                ;

expr            : var ASSIGN expr
                        {
                                $$ = newExprNode(AssignK);
                                $$->attr.op = ASSIGN;
                                $$->lineno = lineno;
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
                                $$ = newExprNode(IdK);
                                $$->attr.name = copyString(tokenString);
                                $$->lineno = lineno;
                        }
                | ID
                        {
                                savedName = copyString(tokenString);
                                savedLineNo = lineno;
                        }
                  LBRAC expr RBRAC
                        {
                                $$ = newExprNode(IdK);
                                $$->attr.name = savedName;
                                $$->lineno = savedLineNo;
                                $$->child[0] = $4;
                        }
                ;

simple_expr     : add_expr relop add_expr
                        {
                                $$ = newExprNode(OpK);
                                $$->attr.op = savedOp;
                                $$->lineno = lineno;
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
                                savedOp = GTE;
                        }
                | GT
                        {
                                savedOp = GT;
                        }
                | LTE
                        {
                                savedOp = LTE;
                        }
                | LT
                        {
                                savedOp = LT;
                        }
                | EQ
                        {
                                savedOp = EQ;
                        }
                | NEQ
                        {
                                savedOp = NEQ;
                        }
                ;

add_expr        : add_expr addop term
                        {
                                $$ = newExprNode(OpK);
                                $$->attr.op = savedOp;
                                $$->lineno = lineno;
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
                                savedOp = PLUS;
                        }
                | MINUS
                        {
                                savedOp = MINUS;
                        }
                ;

term            : term mulop factor
                        {
                                $$ = newExprNode(OpK);
                                $$->attr.op = savedOp;
                                $$->lineno = lineno;
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
                                savedOp = TIMES;
                        }
                | OVER
                        {
                                savedOp = OVER;
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
                                $$ = newExprNode(ConstK);
                                $$->attr.val = copyValue(tokenString);
                                $$->lineno = lineno;
                        }
                ;

call            : ID
                        {
                                savedName = copyString(tokenString);
                                savedLineNo = lineno;
                        }
                  LPAREN args RPAREN
                        {
                                $$ = newExprNode(FunCallK);
                                $$->attr.name = savedName;
                                $$->lineno = savedLineNo;
                                $$->child[0] = $4;
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
