%{
#include <stdio.h>
#include "dlparser.h"
#include "dlsolver.h"
  
extern int yylex(void);
extern char * yytext;

extern AtomSet gatom;

int yyerror(char * s)
{
  printf("parse error! \n");
  return 0;
}

%}

%union {
  ParseNode *pn;
  char * s;
}

%token <s> FULLSTOP
%token <s> SLASH
%token <s> COLON
%token <s> LPAREN
%token <s> RPAREN
%token <s> AMPERSAND
%token <s> VERTICAL
%token <s> TILDE
%token <s> COMMA
%token <s> IDENTITY
%token <s> FCONT_STRING

%type <pn> dl statements statement default
  prerequisite justifications conclusions
  formulas formula

%left AMPERSAND VERTICAL
%right TILDE

%%
dl
  : statements {}
;

statements
  : statements statement FULLSTOP {}
  | {}
;

statement
  : formula  {
    $1->add_bktheory();
    delete $1;
    }
  | default  {
    $1->add_default();
    delete $1;
    }
;

default
  : prerequisite COLON justifications SLASH conclusions {
    $$ = new ParseNode(PT_DEFAULT);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->child[2] = $5;
    if ($1)
      $$->sval = $1->sval;
    $$->sval += " : ";
    if ($3)
      $$->sval += $3->sval;
    $$->sval += " / ";
    if ($5)
      $$->sval += $5->sval;
    }
;

prerequisite
  : formulas {
    $$ = $1;
    }
  | {
    $$ = NULL;
    }
;
justifications
  : formulas {
    $$ = $1;
    }
  | {
    $$ = NULL;
    }
;
conclusions
  : formulas {
    $$ = $1;
    }
  | {
    $$ = NULL;
    }
;

formulas
  : formulas COMMA formula {
    $$ = new ParseNode(PT_FORMULAS);
    $$->child[0] = $3;
    $$->next = $1;
    $$->sval = $1->sval + ", " + $3->sval;
    }
  | formula{
    $$ = new ParseNode(PT_FORMULAS);
    $$->child[0] = $1;
    $$->sval = $1->sval;
    $$->next = NULL;
  }
;

formula
  : formula AMPERSAND formula {
    $$ = new ParseNode(PT_FORMULA);
    $$->ival = FOR_CONJUNCTION;
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->sval = "(" + $1->sval + "&" + $3->sval + ")";
    }
  | formula VERTICAL formula {
    $$ = new ParseNode(PT_FORMULA);
    $$->ival = FOR_DISJUNCTION;
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->sval = "(" + $1->sval + "|" + $3->sval + ")";
    }
  | TILDE formula {
    $$ = new ParseNode(PT_FORMULA);
    $$->ival = FOR_NEGATION;
    $$->child[0] = $2;
    $$->sval = "~" + $2->sval;
    }
  | LPAREN formula RPAREN {
    $$ = $2;
    $$->sval = $2->sval;
    }
  | IDENTITY  {
    $$ = new ParseNode(PT_FORMULA, gatom.add_atom(yytext));
    $$->sval = yytext;
    }
;

%%


