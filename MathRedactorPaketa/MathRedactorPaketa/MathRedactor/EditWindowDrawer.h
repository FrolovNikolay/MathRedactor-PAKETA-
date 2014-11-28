// Автор: Николай Фролов.

// Описание: Класс, отвечающий за отрисовку в окнах класса EditWindow.

#pragma once

#include <vector>
#include "LineOfSymbols.h"
#include "ItemSelector.h"

class CEditWindowDrawer {
public:
	CEditWindowDrawer( const std::vector<CLineOfSymbols>& _content, const CItemSelector& _selector,
		int _hScrollUnit, int _vScrollUnit, int _simpleSymbolHeight );
	CEditWindowDrawer( const CEditWindowDrawer& );
	~CEditWindowDrawer();

	CEditWindowDrawer& operator = ( const CEditWindowDrawer& rhs );

	// Выполнить перерисовку окна.
	void RePaint( HWND windowHandle ) const;

private:
	// Строки окна-владельца механизма.
	const std::vector<CLineOfSymbols>& content;
	// Механизм выделения окна-владельца
	const CItemSelector& selector;

	// Константы связанные с скроллом окна-владельца.
	const int hScrollUnit;
	const int vScrollUnit;
	// Высота простых символов в окне-владельце.
	const int simpleSymbolHeight;

	// Кэшированные GDI-объекты, для уменьшения нагрузки на ядро.
	mutable HDC tempHDC;
	mutable HBITMAP tempBitmap;
	mutable HBRUSH backgroundBrush;
	mutable HFONT font;
	mutable HPEN linePen;
	mutable HBRUSH selectionBrush;
	mutable HPEN selectionPen;

	// Отрисовать в окне выделение.
	void drawSelection( HWND ) const;

	// Отрисовать выделение в редакторе в зависимости от заданных начального и конечного символов.
	void drawGlobalSelection( const CSymbolPosition& start, const CSymbolPosition& end, HWND ) const;

	// Отрисовать выделение в редакторе в пределах строки в зависимости от заданных начального и конечного символов.
	void drawLocalSelection( const CSymbolPosition& start, const CSymbolPosition& end, HWND ) const;

	// Получить размеры и смещение скролла окна-владельца.
	void getWindowInfo( int& leftTopX, int& leftTopY, int& width, int& height, HWND windowHandle ) const;
};