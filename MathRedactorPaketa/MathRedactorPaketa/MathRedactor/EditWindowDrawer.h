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
};