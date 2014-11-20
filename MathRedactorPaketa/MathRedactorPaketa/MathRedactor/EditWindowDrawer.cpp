// Автор: Фролов Николай.

#include "EditWindowDrawer.h"
#include <cassert>

CEditWindowDrawer::CEditWindowDrawer( const std::vector<CLineOfSymbols>& _content, const CItemSelector& _selector,
	int _hScrollUnit, int _vScrollUnit, int _simpleSymbolHeight )
		: content( _content )
		, selector( _selector )
		, hScrollUnit( _hScrollUnit )
		, vScrollUnit( _vScrollUnit )
		, simpleSymbolHeight( _simpleSymbolHeight )
{

	backgroundBrush = ::CreateSolidBrush( RGB( 255, 255, 255 ) );
	font = ::CreateFont( simpleSymbolHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Georgia" );
	linePen = ::CreatePen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
}

CEditWindowDrawer::CEditWindowDrawer( const CEditWindowDrawer& src )
	: content( src.content )
	, selector( src.selector )
	, hScrollUnit( src.hScrollUnit )
	, vScrollUnit( src.vScrollUnit )
	, simpleSymbolHeight( src.simpleSymbolHeight )
{
	backgroundBrush = ::CreateSolidBrush( RGB( 255, 255, 255 ) );
	font = ::CreateFont( simpleSymbolHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Georgia" );
	linePen = ::CreatePen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
}

CEditWindowDrawer::~CEditWindowDrawer()
{
	::DeleteObject( backgroundBrush );
	::DeleteObject( linePen );
	::DeleteObject( font );
}

CEditWindowDrawer& CEditWindowDrawer::operator = ( const CEditWindowDrawer& src )
{
	if( this == &src ) {
		return *this;
	}
	CEditWindowDrawer temp( src );
	std::swap( *this, temp );
	return *this;
}

void CEditWindowDrawer::RePaint( HWND windowHandle ) const
{
	PAINTSTRUCT paintInfo;
	HDC displayHandle = ::BeginPaint( windowHandle, &paintInfo );
	assert( displayHandle != 0 );

	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
	int posX = - scrollInfo.nPos * hScrollUnit;
	::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
	int posY = - scrollInfo.nPos * vScrollUnit;

	//Получаем размеры клиентского окна
	RECT clientRect;
	::GetClientRect( windowHandle, &clientRect );

	int width = clientRect.right;
	int height = clientRect.bottom;

	tempHDC = CreateCompatibleDC( displayHandle );
	tempBitmap = CreateCompatibleBitmap( displayHandle, width, height );
	HBITMAP oldBitmap = static_cast<HBITMAP>( ::SelectObject( tempHDC, tempBitmap ) );

	//Фон
	HBRUSH oldBgBrush = static_cast<HBRUSH>( ::SelectObject( tempHDC, backgroundBrush ) );
	::Rectangle( tempHDC, 0, 0, width, height );
	::SelectObject( tempHDC, oldBgBrush );

	if( selector.HasSelection() ) {
		selector.MakeSelection( tempHDC, width, height, posX, posY );
	}

	HFONT oldFont = static_cast<HFONT>( ::SelectObject( tempHDC, font ) );

	// Настройка кисти для линии ( рисование дроби ).
	HPEN oldLinePen = static_cast<HPEN>( ::SelectObject( tempHDC, linePen ) );

	// Фон текста прозрачный, для выделения.
	::SetBkMode( tempHDC, TRANSPARENT );

	for( int i = 0; i < static_cast<int>( content.size() ); ++i ) {
		content[i].Draw( tempHDC, posX, posY );
		posY += content[i].GetHeight();
	}

	::SetBkMode( tempHDC, OPAQUE );

	::SelectObject( tempHDC, oldLinePen );
	::SelectObject( tempHDC, oldFont );

	::BitBlt( displayHandle, 0, 0, width, height, tempHDC, 0, 0, SRCCOPY);
	::DeleteObject( oldBitmap );
	::DeleteObject( tempHDC );
	::DeleteObject( tempBitmap );

	::EndPaint( windowHandle, &paintInfo );
}