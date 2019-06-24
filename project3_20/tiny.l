%{
#include "globals.h"
#include "util.h"
#include <stdio.h>

/* follow standard C identifier length */
#define MAXTOKENLEN 31
char token_string[MAXTOKENLEN+1];
char id_string[MAXTOKENLEN+1];
static void yyerror(char const *);
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"if"            { return IF; }
"else"          { return ELSE; }
"int"           { return INT; }
"return"        { return RETURN; }
"void"          { return VOID; }
"while"         { return WHILE; }
"=="            { return EQ; }
"="             { return ASSIGN; }
"!="            { return NEQ; }
"<"             { return LT; }
"<="            { return LTE; }
">"             { return GT; }
">="            { return GTE; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return OVER; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LBRAC; }
"]"             { return RBRAC; }
"{"             { return LCURLY; }
"}"             { return RCURLY; }
","             { return COMMA; }
";"             { return SEMI; }
"/*"            { 
                  register int c;
                  for (;;) { 
                          while ((c = input()) != '*' && c != EOF)
                                  lineno += c == '\n';
                          if (c == '*') {
                                  while ((c = input()) == '*');
                                  lineno += c == '\n';
                                  if ( c == '/' )
                                          break;
                          }
                          if (c == EOF) {
                                  yyerror("Comment Error");
                                  return ERROR;
                          }
                  }
                }
<<EOF>>         { return ENDFILE; }
{number}        { return NUM; }
{identifier}    { return ID; }
{newline}       { lineno++; }
{whitespace}    { /* skip whitespace */ }
.               { return ERROR; }

%%

token_t get_token(void)
{
        static int first_time = TRUE;
        token_t current_token;
        if (first_time) {
                first_time = FALSE;
                lineno++;
                yyin = source;
                yyout = listing;
        }
        current_token = yylex();
        strncpy(id_string, token_string, MAXTOKENLEN);
        strncpy(token_string, yytext, MAXTOKENLEN);
        if (TraceScan) {
                fprintf(listing, "\t%d", lineno);
                print_token(current_token, token_string);
        }
        return current_token;
}

int yywrap()
{
        /* actions for reaching EOF */
}

static void yyerror(char const *msg)
{
        strcpy(yytext, msg);
}
