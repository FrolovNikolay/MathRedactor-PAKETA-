// Автор: Азат Давлетшин
// Описание: Реализация LR-парсера для Latex

#include "LatexParser.h "
#include <iostream>
#include <exception>

using namespace std;

// ParsingTable.cpp
extern const int Err;
extern vector<vector<int> > ParsingTable;

void CLatexParser::Init()
{
	while( states.size() > 0 ) {
		states.pop();
	}

	// Стартовое состояние
	CParserState state;
	state.Id = 0;
	state.Node = 0;
	state.Token = TT_NOTFOUND;
	states.push( state );
}

void CLatexParser::TryParse( shared_ptr<CLatexState> state )
{
	if( !state->IsDetached() ) {
		// Стоп-символ не отсоединил лексему, нужно поглотить еще один символ
		return;
	}

	if( state->GetType() == TT_NOTFOUND ) {
		// Состояние не финальное, а символ отсоединен. Значит ошибка в вводе
		throw invalid_argument( "Lexical error" );
	}

	if( state->GetType() == TT_SPACE ) {
		//delete state;
		return;
	}

	Parse( state );
	//delete state;
}

void CLatexParser::Parse( shared_ptr<CLatexState> state )
{
	int currentState = states.top().Id;
	TTokenType currentToken = state->GetType();
	int action = ParsingTable[currentState][currentToken];


	if( action == Err ) {
		throw invalid_argument( "Syntax error" );
	}

	if( action < 0 ) {
		// Свертка (reduce)
		shared_ptr<CLatexExpState> reducedState = applyRule( -action );
		TryParse( reducedState );
		// Если попался нетерминал, то пришедший токен не обработан, поэтому запускаем обработку еще раз
		return Parse( state );
	}

	CParserState newParserState;
	newParserState.Id = action;
	newParserState.Token = currentToken;

	if( currentToken == TT_NONTERMINAL ) {
		// Парсим нетерминальный символ
		shared_ptr<CLatexExpState> expState = dynamic_pointer_cast<CLatexExpState>( state );
		assert( expState != 0 );
		newParserState.Node = expState->GetNode(); 
		states.push( newParserState );
		return;
	}

	newParserState.Node = 0;

	if( currentToken == TT_ID ) {
		shared_ptr<CLatexIDState> idState = dynamic_pointer_cast<CLatexIDState>( state );
		assert( idState != 0 );
		newParserState.Val = idState->GetId();
	}

	states.push( newParserState );
}

shared_ptr<CLatexExpState> CLatexParser::applyRule( int rule )
{
	shared_ptr<MathObj> node = 0;
	switch( rule )
	{
		case 1:
			// Exp * Exp
			node = shared_ptr<FormulaObj>(new FormulaObj( NT_MULTCM ));
			popNonTerminal( node );
			popTerminal( TT_MULTIPLY );
			popNonTerminal( node, false );
			return shared_ptr<CLatexExpState>(new CLatexExpState( node ));
		case 2:
			// Exp + Exp
			node = shared_ptr<FormulaObj>(new FormulaObj( NT_PLUS ));
			popNonTerminal( node );
			popTerminal( TT_PLUS );
			popNonTerminal( node, false );
			return shared_ptr<CLatexExpState>(new CLatexExpState( node ));
		case 3:
			// FRAC { Exp } { Exp }
			node = shared_ptr<FormulaObj>(new FormulaObj( NT_DIV ));
			popTerminal( TT_RCURLYBRACE );
			popNonTerminal( node );
			popTerminal( TT_LCURLYBRACE );
			popTerminal( TT_RCURLYBRACE );
			popNonTerminal( node, false );
			popTerminal( TT_LCURLYBRACE );
			popTerminal( TT_FRAC );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 4:
			// Exp - Exp
			node = shared_ptr<FormulaObj>(new FormulaObj( NT_MINUS ));
			popNonTerminal( node );
			popTerminal( TT_MINUS );
			popNonTerminal( node, false );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 5:
			// -Exp
			node = shared_ptr<FormulaObj>(new FormulaObj( NT_UMINUS ));
			popNonTerminal( node );
			popTerminal( TT_MINUS );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 6:
			// ID
			node = shared_ptr<ParamObj>(new ParamObj( states.top().Val ));
			popTerminal( TT_ID );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 7:
			// Exp ^ { Exp }
			node = shared_ptr<FormulaObj>(new FormulaObj( NT_POW ));
			popTerminal( TT_RCURLYBRACE );
			popNonTerminal( node );
			popTerminal( TT_LCURLYBRACE );
			popTerminal( TT_POW );
			popNonTerminal( node, false );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 8:
			// SQRT { Exp }
			node = shared_ptr<FormulaObj>(new FormulaObj( NT_ROOT ));
			popTerminal( TT_RCURLYBRACE );
			popNonTerminal( node );
			popTerminal( TT_LCURLYBRACE );
			popTerminal( TT_SQRT );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 9:
			// SQRT [ Exp ] { Exp }
			node = shared_ptr<FormulaObj>(new FormulaObj(NT_ROOT));
			popTerminal( TT_RCURLYBRACE );
			popNonTerminal( node );
			popTerminal( TT_LCURLYBRACE );
			popTerminal( TT_RBRACKET );
			popNonTerminal( node, false );
			popTerminal( TT_LBRACKET );
			popTerminal( TT_SQRT );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 10:
			// ( Exp )
			popTerminal( TT_RBRACE );

			assert( states.top().Token == TT_NONTERMINAL );
			node = states.top().Node;
			states.pop();

			popTerminal( TT_LBRACE );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 11:
			// SUM _ { Exp } ^ { Exp } Exp
			node = shared_ptr<FormulaObj>(new FormulaObj(NT_SUM));
			popNonTerminal( node );
			popTerminal( TT_RCURLYBRACE );
			popNonTerminal( node, false );
			popTerminal( TT_LCURLYBRACE );
			popTerminal( TT_POW );
			popTerminal( TT_RCURLYBRACE );
			popNonTerminal( node, false );
			popTerminal( TT_LCURLYBRACE );
			popTerminal( TT_UNDERSCORE );
			popTerminal( TT_SUM );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 12:
			// PROD _ { Exp } ^ { Exp } Exp
			node = shared_ptr<FormulaObj>(new FormulaObj(NT_PROD));
			popNonTerminal( node );
			popTerminal( TT_RCURLYBRACE );
			popNonTerminal( node, false );
			popTerminal( TT_LCURLYBRACE );
			popTerminal( TT_POW );
			popTerminal( TT_RCURLYBRACE );
			popNonTerminal( node, false );
			popTerminal( TT_LCURLYBRACE );
			popTerminal( TT_UNDERSCORE );
			popTerminal( TT_PROD );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 13:
			// SIN Exp
			node = shared_ptr<FormulaObj>(new FormulaObj(NT_SIN));
			popNonTerminal( node );
			popTerminal( TT_SIN );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 14:
			// COS Exp
			node = shared_ptr<FormulaObj>(new FormulaObj(NT_COS));
			popNonTerminal( node );
			popTerminal( TT_COS );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 15:
			// TAN Exp
			node = shared_ptr<FormulaObj>(new FormulaObj(NT_TAN));
			popNonTerminal( node );
			popTerminal( TT_TAN );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		case 16:
			// COT Exp
			node = shared_ptr<FormulaObj>(new FormulaObj(NT_COT));
			popNonTerminal( node );
			popTerminal( TT_COT );
			return shared_ptr<CLatexExpState>(new CLatexExpState(node));
		default:
			assert( false );
	}

	return 0;
}

void CLatexParser::popTerminal( TTokenType type ) 
{
	assert( states.top().Token == type );
	states.pop();
}

void CLatexParser::popNonTerminal( shared_ptr<MathObj> node, bool pushBack )
{
	assert( states.top().Token == TT_NONTERMINAL );

	shared_ptr<FormulaObj> formulaNode = dynamic_pointer_cast<FormulaObj>( node );
	assert( formulaNode != 0 );
	if( pushBack ) {
		formulaNode->params.push_back( states.top().Node );
	} else {
		formulaNode->params.insert( formulaNode->params.begin(), states.top().Node );
	}
	states.pop();
}

shared_ptr<MathObj> CLatexParser::End()
{
	// Проталкиваем EOF
	shared_ptr<CLatexEndState> endState (new CLatexEndState());
	TryParse( endState );

	assert( states.size() == 3 );

	states.pop();

	assert( states.top().Token == TT_NONTERMINAL );

	shared_ptr<FormulaObj> root ( new FormulaObj( NT_MAIN ));
	root->params.push_back( states.top().Node );
	states.pop();

	assert( states.top().Id == 0 );

	return root;
}