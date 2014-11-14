
%{
#include "dlparser.h"
#include "parse.h"
%}

char	        [_0-9a-zA-Z]
id		{char}+
whitespace      [\n\t\r ]*

%%
{id} 		{return IDENTITY;}
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
%%

int yywrap()
{
  return 1;
}
