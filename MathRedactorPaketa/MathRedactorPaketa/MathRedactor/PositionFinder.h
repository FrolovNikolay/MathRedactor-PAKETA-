// Автор: Фролов Николай.

// Описание: Класс, реализует алгоритм поиска позиции символа во внутренней структуре по координатам.

#pragma once

#include "SymbolPosition.h"
#include "LineOfSymbols.h"

class CPositionFinder {
public:
	explicit CPositionFinder( const std::vector<CLineOfSymbols>& _content ) : content( _content )
	{ }

	// Получить позицию, соответствующую указанной точке.
	CSymbolPosition* FindPosition( int x, int y, int firstEnableSymbol = 0, CSymbolPosition* baseLine = 0 ) const;

private:
	// Содержимое окна-владельца.
	const std::vector<CLineOfSymbols>& content;

	// Попытаться уточнить связанную с координатами позицию.
	CSymbolPosition* exactPosition( int x, int y, CSymbolPosition* parent ) const;

	// Найти значение позиции символа внутри строки.
	CSymbolPosition* positionInLine( int x, int y, CSymbolPosition* line ) const;

	// Проверям попадает ли данная точка в данную линию.
	static bool IsLineContainPoint( const CLineOfSymbols* line, int x, int y );
};