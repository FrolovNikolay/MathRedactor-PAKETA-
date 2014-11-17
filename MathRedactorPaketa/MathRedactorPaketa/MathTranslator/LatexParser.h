// Автор: Азат Давлетшин
// Описание: Парсер (LR) превращает поток лексем из Latex в синтаксическое дерево

#pragma once

#include "LatexLexerStates.h"
#include "MathFormObj.h"
#include <stack>

// Если в результате свертки получили нетерминал, то нужно его протолкнуть как обычный токен,
// поэтому требуется еще одно состояние лексера
class CLatexExpState : public CLatexState {
public:
	CLatexExpState( shared_ptr<MathObj> _node ) { type = TT_NONTERMINAL; node = _node; isDetached = true; }

	shared_ptr<MathObj> GetNode() { return node; }

	// Этот метод не вызвается
	virtual shared_ptr<CLatexState> Accept( char symbol ) { assert( false ); return shared_ptr<CLatexState>(0); }

private:
	shared_ptr<MathObj> node;
};

// LR-парсер для Latex. Для работы необходимо проинициализировать методом Init(). Когда строка прочитана, необходимо
// протолкнуть EOF с помощью метода End(), который возращает результат парсинга в виде корня синтаксического дерева
class CLatexParser {
public:
	void TryParse( shared_ptr<CLatexState> state );
	void Parse( shared_ptr<CLatexState> state );
	// Протолкнуть токен EOF и получить корень синтаксического дерева
	shared_ptr<MathObj> End();

	// Инициализация парсера
	void Init();

private:
	// Состояние LR-паресера. Хранит в себе терминал или нетерминал, а также свой номер
	// в таблице переходов
	struct CParserState {
		int Id;
		TTokenType Token;
		shared_ptr<MathObj> Node;
		std::string Val; // Для токена TT_ID
	};

	shared_ptr<CLatexExpState> applyRule( int rule );
	void popTerminal( TTokenType type );
	void popNonTerminal( shared_ptr<MathObj> node, bool pushBack = true );
	std::stack<CParserState> states;
};