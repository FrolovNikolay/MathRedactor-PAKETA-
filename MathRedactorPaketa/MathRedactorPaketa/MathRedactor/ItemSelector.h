// Автор: Фролов Николай.

// Описание: класс отвечающий за хранение информации о выделении символов в окне.

#pragma once

#include "SymbolPosition.h"
#include "PositionFinder.h"

class CItemSelector {
public:
	CItemSelector( const CPositionFinder&, const std::vector<CLineOfSymbols>& );

	typedef std::pair<CSymbolPosition, CSymbolPosition> CSymbolInterval;

	// Установка начальной позиции курсора
	void SetStartPosition( int x, int y, int firstEnableSymbol = 0 );
	// Установка текущей позиции курсора
	void SetCurrentPosition( int x, int y, int firstEnableSymbol = 0 );
	// Проверить наличие выделения.
	bool HasSelection() const { return start != 0 && end != 0; }
	// Снять выделение.
	void ResetSelection();
	// Информация о выделенных объектах.
	CSymbolInterval GetSelectionInfo() const;

private:
	const CPositionFinder& finder;
	const std::vector<CLineOfSymbols>& content;

	std::shared_ptr<CSymbolPosition> start;
	std::shared_ptr<CSymbolPosition> end;

	void goToNextSymbol( int& lineIdx, CSymbolPosition& position ) const;

	void goToPrevSymbol( int& lineIdx, CSymbolPosition& position ) const;
};