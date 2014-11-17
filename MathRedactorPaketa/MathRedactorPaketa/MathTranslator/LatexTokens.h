// Автор: Азат Давлетшин
// Описание: токены

#pragma once

#include <string>
#include <unordered_map>

enum TTokenType {
	TT_MULTIPLY = 0,
	TT_PLUS = 1,
	TT_MINUS = 2,
	TT_LBRACKET = 3,
	TT_RBRACKET = 4,
	TT_POW = 5,
	TT_UNDERSCORE = 6,
	TT_LCURLYBRACE = 7,
	TT_RCURLYBRACE = 8,
	TT_PROD = 9,
	TT_SUM = 10,
	TT_COT = 11,
	TT_TAN = 12,
	TT_COS = 13,
	TT_SIN = 14,
	TT_ID = 15,
	TT_FRAC = 16,
	TT_SQRT = 17,
	TT_LBRACE = 18,
	TT_RBRACE = 19,
	TT_NONTERMINAL = 20,
	TT_EOF = 21,
	TT_SPACE,
	TT_NOTFOUND
};

class CTokenFinder {
public:
	CTokenFinder();

	TTokenType FindFunction( const std::string& str ) const;
	
private:
	std::unordered_map<std::string, TTokenType> functions;
};

