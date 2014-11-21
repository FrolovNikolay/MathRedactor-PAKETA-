// Автор: Фролов Николай.

// Описание: класс отвечающий за хранение информации о выделении символов в окне.

#pragma once

#include "SymbolPosition.h"
#include "PositionFinder.h"

class CItemSelector {
public:
	CItemSelector( const CPositionFinder& );

	// Установка начальной позиции курсора
	void SetStartPosition( int x, int y );
	// Установка текущей позиции курсора
	void SetCurrentPosition( int x, int y );
	// Проверить наличие выделения.
	bool HasSelection() const { return start != 0 && end != 0; }
	// Снять выделение.
	void ResetSelection() { delete start; start = 0; delete end; end = 0; }
	// Информация о выделенных объектах.
	void GetSelectionInfo( const CSymbolPosition*& _start, const CSymbolPosition*& _end ) const { _start = start; _end = end; }

private:
	const CPositionFinder& finder;

	const CSymbolPosition* start;
	const CSymbolPosition* end;
};