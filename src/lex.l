
%{
#include "dlparser.h"
#include "parse.h"
%}

num	        [0-9][0-9]*
id		[_a-zA-Z][_0-9a-zA-Z]*
whitespace      [\n\t\r ]*

%%
{id} 		{return IDENTITY;}
{num}           {return NUMBER;}
{whitespace}    {/* skip whitespace */}
"."		{return FULLSTOP;}
"/"		{return SLASH;}
":"		{return COLON;}
"("		{return LPAREN;}
")"		{return RPAREN;}
"&"		{return AMPERSAND;}
"|"		{return VERTICAL;}
"~"		{return TILDE;}
","		{return COMMA;}
"["             {return LSQU;}
"]"             {return RSQU;}
"@"             {return AT;}
%%

int yywrap()
{
  return 1;
}
