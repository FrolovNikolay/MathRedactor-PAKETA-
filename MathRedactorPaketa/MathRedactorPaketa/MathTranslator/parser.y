%{
#include <iostream>

using namespace std;

extern "C" int yylex();
extern "C" int yyparse();
%}

%left '+' '-'

%left SUM PROD

%left SIN COS TAN COT

%left '*'

%left UMINUS

%left '^'

%token ID

%token FRAC

%token SQRT

%token SUM

%token PROD

%token TAN

%token COT

%token COS

%token SIN

%%
Exp:
	Exp '*' Exp
	| Exp '+' Exp
	| FRAC '{' Exp '}' '{' Exp '}'
	| Exp '-' Exp
	| '-' Exp %prec UMINUS
	| ID
	| Exp '^' '{' Exp '}'
	| SQRT '{' Exp '}'
	| SQRT '[' Exp ']' '{' Exp '}'
	| "(" Exp ")"
	| SUM '_' '{' Exp '}' '^' '{' Exp '}' Exp
	| PROD '_' '{' Exp '}' '^' '{' Exp '}' Exp
	| SIN Exp
	| COS Exp
	| TAN Exp
	| COT Exp
	;
	
%%

int main()
{
	while( yyparse() != 0 );
	return 0;
}