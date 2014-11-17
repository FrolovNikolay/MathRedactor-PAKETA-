// Автор: Азат Давлетшин
// Описание: Реализация состояний лексера для Latex

#include "LatexLexerStates.h"
#include <algorithm>
#include <assert.h>

using namespace std;

static const CTokenFinder TokenFinder;

shared_ptr<CLatexState> CLatexState::TryStopSymbol( char symbol )
{
	switch( symbol ) 
	{
		case '\\':
			return shared_ptr<CLatexFuncState>(new CLatexFuncState());
		case '(':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_LBRACE));
		case ')':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState( TT_RBRACE ));
		case '{':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_LCURLYBRACE));
		case '}':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_RCURLYBRACE));
		case '[':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_LBRACKET));
		case ']':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_RBRACKET));
		case '+':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_PLUS));
		case '-':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_MINUS));
		case '*':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_MULTIPLY));
		case '^':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_POW));
		case ' ':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_SPACE));
		case '_':
			return shared_ptr<CLatexSingleState>(new CLatexSingleState(TT_UNDERSCORE));
		default:
			return 0;
	}
}

shared_ptr<CLatexState> CLatexIDState::Accept( char symbol )
{
	shared_ptr<CLatexState> tryStopSymbol = TryStopSymbol( symbol );
	if( tryStopSymbol != 0 ) {
		isDetached = true;
		return tryStopSymbol;
	}

	id += symbol; 
	return shared_from_this();
}

shared_ptr<CLatexState> CLatexFuncState::Accept( char symbol )
{
	shared_ptr<CLatexState> tryStopSymbol = TryStopSymbol( symbol );
	if( tryStopSymbol != 0 ) {
		isDetached = true;
		return tryStopSymbol;
	}

	eatenStr += symbol;
	type = TokenFinder.FindFunction( eatenStr );

	return shared_from_this();
}

shared_ptr<CLatexState> CLatexSingleState::Accept( char symbol )
{
	isDetached = true;

	shared_ptr<CLatexState> tryStopSymbol = TryStopSymbol( symbol );
	if( tryStopSymbol != 0 ) {
		return tryStopSymbol;
	}

	return shared_ptr<CLatexIDState>(new CLatexIDState( symbol ));
}