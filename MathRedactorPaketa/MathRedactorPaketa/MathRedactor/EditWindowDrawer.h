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

	void RePaint( HWND windowHandle ) const;

private:
	const std::vector<CLineOfSymbols>& content;
	const CItemSelector& selector;
	const int hScrollUnit;
	const int vScrollUnit;
	const int simpleSymbolHeight;

	mutable HDC tempHDC;
	mutable HBITMAP tempBitmap;
	mutable HBRUSH backgroundBrush;
	mutable HFONT font;
	mutable HPEN linePen;
	mutable HBRUSH selectionBrush;
	mutable HPEN selectionPen;

	void drawSelection( HWND ) const;

	void drawGlobalSelection( const CSymbolPosition& start, const CSymbolPosition& end, HWND ) const;

	void drawLocalSelection( const CSymbolPosition& start, const CSymbolPosition& end, HWND ) const;

	void getWindowInfo( int& leftTopX, int& leftTopY, int& width, int& height, HWND windowHandle ) const;
};