// Автор: Азат Давлетшин
// Описание: Реализация класса, который провереят существовании функции и возращает токен

#include "LatexTokens.h"

using namespace std;

CTokenFinder::CTokenFinder()
{
	functions["cos"] = TT_COS;
	functions["cot"] = TT_COT;
	functions["frac"] = TT_FRAC;
	functions["prod"] = TT_PROD;
	functions["sin"] = TT_SIN;
	functions["sum"] = TT_SUM;
	functions["tan"] = TT_TAN;
	functions["sqrt"] = TT_SQRT;
}

TTokenType CTokenFinder::FindFunction( const string& str ) const
{
	unordered_map<string, TTokenType>::const_iterator it = functions.find( str );
	
	if( it == functions.end() ) {
		return TT_NOTFOUND;
	}

	return it->second;
}