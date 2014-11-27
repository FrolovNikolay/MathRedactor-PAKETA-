// Автор: Фролов Николай.

#include "EditWindowDrawer.h"
#include "SymbolPosition.h"
#include <memory>
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
	selectionBrush = ::CreateSolidBrush( RGB( 100, 100, 255 ) );
	selectionPen = CreatePen( PS_SOLID, 1, RGB( 100, 100, 255 ) );
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
	selectionBrush = ::CreateSolidBrush( RGB( 100, 100, 255 ) );
	selectionPen = CreatePen( PS_SOLID, 1, RGB( 100, 100, 255 ) );
}

CEditWindowDrawer::~CEditWindowDrawer()
{
	::DeleteObject( backgroundBrush );
	::DeleteObject( linePen );
	::DeleteObject( font );
	::DeleteObject( selectionBrush );
	::DeleteObject( selectionPen );
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

	int leftTopX;
	int leftTopY;
	int width;
	int height;
	getWindowInfo( leftTopX, leftTopY, width, height, windowHandle );

	tempHDC = CreateCompatibleDC( displayHandle );
	tempBitmap = CreateCompatibleBitmap( displayHandle, width, height );
	HBITMAP oldBitmap = static_cast<HBITMAP>( ::SelectObject( tempHDC, tempBitmap ) );
	HBRUSH oldBgBrush = static_cast<HBRUSH>( ::SelectObject( tempHDC, backgroundBrush ) );

	::Rectangle( tempHDC, 0, 0, width, height );
	::SelectObject( tempHDC, oldBgBrush );

	drawSelection( windowHandle );

	HFONT oldFont = static_cast<HFONT>( ::SelectObject( tempHDC, font ) );
	HPEN oldLinePen = static_cast<HPEN>( ::SelectObject( tempHDC, linePen ) );
	::SetBkMode( tempHDC, TRANSPARENT );

	int offsetY = 0;
	for( int i = 0; i < static_cast<int>( content.size() ); ++i ) {
		content[i].Draw( tempHDC, -leftTopX, -leftTopY + offsetY );
		offsetY += content[i].GetHeight();
	}

	::SetBkMode( tempHDC, OPAQUE );

	::SelectObject( tempHDC, oldLinePen );
	::SelectObject( tempHDC, oldFont );
	::BitBlt( displayHandle, 0, 0, width, height, tempHDC, 0, 0, SRCCOPY );

	::DeleteObject( oldBitmap );
	::DeleteObject( tempHDC );
	::DeleteObject( tempBitmap );
	::EndPaint( windowHandle, &paintInfo );
}

void CEditWindowDrawer::drawSelection( HWND windowHandle ) const
{
	if( selector.HasSelection() ) {
		CItemSelector::CSymbolInterval interval = selector.GetSelectionInfo();
		CSymbolPosition start = interval.first;
		CSymbolPosition end = interval.second;
		if( start.Index == -1 || end.Index == -1 ) {
			return;
		}
		if( start.GetParent() == 0 && end.GetParent() == 0 ) {
			drawGlobalSelection( start, end, windowHandle );
		} else if( start.GetParent() != 0 && end.GetParent() != 0 ) {
			drawLocalSelection( start, end, windowHandle );
		} else {
			// Начало в базовой строке, а конец нет, либо наоборот. Такого не должно произойти.
			assert( false );
		}
	}
}

void CEditWindowDrawer::drawGlobalSelection( const CSymbolPosition& start, const CSymbolPosition& end, HWND windowHandle ) const
{
	if( start.CurrentLine == end.CurrentLine ) {
		drawLocalSelection( start, end, windowHandle );
	} else {
		int startLine = -1;
		while( &content[++startLine] != start.CurrentLine ) { }

		int endLine = -1;
		while( &content[++endLine] != end.CurrentLine ) { }

		drawLocalSelection( start, CSymbolPosition( start.CurrentLine->Length() - 1, start.CurrentLine ), windowHandle );
		for( int i = startLine + 1; i < endLine; ++i ) {
			if( content[i].Length() != 0 ) {
				drawLocalSelection( CSymbolPosition( 0, &content[i] ), CSymbolPosition( content[i].Length() - 1, &content[i] ), windowHandle );
			}
		}
		drawLocalSelection( CSymbolPosition( 0, end.CurrentLine ), end, windowHandle );
	}
}

void CEditWindowDrawer::drawLocalSelection( const CSymbolPosition& start, const CSymbolPosition& end, HWND windowHandle ) const
{
	HBRUSH lastBrush = static_cast<HBRUSH>( ::SelectObject( tempHDC, selectionBrush ) );
	HPEN oldPen = static_cast<HPEN>( ::SelectObject( tempHDC, selectionPen ) );

	int moveX = 0;
	int moveY = 0;
	{
		int width, height;
		getWindowInfo( moveX, moveY, width, height, windowHandle );
	}

	int leftTopX = ( *start.CurrentLine )[start.Index]->GetX() - content[0].GetX() - moveX;
	int rightBotX = ( *end.CurrentLine )[end.Index]->GetX() - content[0].GetX() - moveX;
	int leftTopY = start.CurrentLine->GetY() - content[0].GetY() - moveY;
	int rightBotY = leftTopY + start.CurrentLine->GetHeight();
	if( leftTopX > rightBotX ) {
		std::swap( leftTopX, rightBotX );
		rightBotX += ( *start.CurrentLine )[start.Index]->GetWidth();
	} else {
		rightBotX += ( *end.CurrentLine )[end.Index]->GetWidth();
	}
	::Rectangle( tempHDC, leftTopX, leftTopY, rightBotX, rightBotY );

	::SelectObject( tempHDC, lastBrush );
	::SelectObject( tempHDC, oldPen );
}

void CEditWindowDrawer::getWindowInfo( int& leftTopX, int& leftTopY, int& width, int& height, HWND windowHandle ) const
{
	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;
	::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
	leftTopX = scrollInfo.nPos * hScrollUnit;
	::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
	leftTopY = scrollInfo.nPos * vScrollUnit;

	//Получаем размеры клиентского окна
	RECT clientRect;
	::GetClientRect( windowHandle, &clientRect );
	width = clientRect.right;
	height = clientRect.bottom;
}

