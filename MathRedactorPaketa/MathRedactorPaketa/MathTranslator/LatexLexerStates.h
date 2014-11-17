// Автор: Азат Давлетшин
// Описание: Состояния лексера для Latex

#pragma once

#include <string>
#include <vector>
#include <assert.h>
#include "LatexTokens.h"
#include <memory>

class CConvertLatex;

// Абстрактный класс состояния (лексера)
class CLatexState {
public:
	CLatexState() : type( TT_NOTFOUND ), isDetached( false ) {}
	virtual ~CLatexState( ) {}

	TTokenType GetType( ) const { return type; }
	bool IsDetached( ) const { return isDetached; }
	void Detach() { isDetached = true; }

	virtual std::shared_ptr<CLatexState> Accept( char symbol ) = 0;

	// Пробуем стоп-символы. Если символ не стоповый, то возращаем 0, иначе создаем соотв. состояние
	static std::shared_ptr<CLatexState> TryStopSymbol( char symbol );
	
protected:
	bool isDetached;
	TTokenType type;
};

class CLatexIDState : public CLatexState, public std::enable_shared_from_this<CLatexIDState> {
public:
	CLatexIDState( char firstSymbol ) { type = TT_ID; id += firstSymbol; }

	const std::string& GetId() const { return id; }

	virtual std::shared_ptr<CLatexState> Accept( char symbol );

private:
	std::string id;
};

class CLatexFuncState : public CLatexState, public std::enable_shared_from_this<CLatexFuncState> {
public:
	virtual std::shared_ptr<CLatexState> Accept( char symbol );

private:
	static const std::vector<const std::string> functions;
	std::string eatenStr;
};

class CLatexSingleState : public CLatexState, public std::enable_shared_from_this<CLatexSingleState> {
public:
	CLatexSingleState( TTokenType _type ) { type = _type; }

	virtual std::shared_ptr<CLatexState> Accept( char symbol );
};

// Маркер конца файла
class CLatexEndState : public CLatexState, public std::enable_shared_from_this<CLatexEndState> {
public:
	CLatexEndState() { type = TT_EOF; isDetached = true; }

	// Этот метод не вызвается
	virtual std::shared_ptr<CLatexState> Accept( char symbol ) { assert( false ); return 0; }
};
