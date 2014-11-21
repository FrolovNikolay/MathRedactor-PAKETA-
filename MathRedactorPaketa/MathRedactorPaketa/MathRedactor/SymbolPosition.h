// Автор: Фролов Николай.

// Описание: Структура, отвечающая за хранение позиции во внутренней структуре редактора.

#pragma once

#include "LineOfSymbols.h"
#include <memory>

struct CSymbolPosition {
public:
	// Позиция в строке.
	int Index;
	// Строка, в которой находимся.
	const CLineOfSymbols& CurrentLine;

	CSymbolPosition( int _Index, const CLineOfSymbols& _CurrentLine, const CSymbolPosition* _parentPosition = 0 ) 
		: Index( _Index )
		, CurrentLine( _CurrentLine )
		, parentPosition( _parentPosition )
	{ }

	CSymbolPosition( int _Index, const CSymbolPosition& src )
		: Index( _Index )
		, CurrentLine( src.CurrentLine )
		, parentPosition( src.parentPosition )
	{ }

	const CSymbolPosition* GetParent() const { return parentPosition.get(); };
private:
	// 0, если находимся в базовой строке.
	std::shared_ptr<const CSymbolPosition> parentPosition;
};