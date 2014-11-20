#include "RootSymbol.h"
#include <assert.h>
#include <wingdi.h>
#include <Windows.h>

const wchar_t CRootSymbol::symbol[] = { L'\u221A' };

CRootSymbol::CRootSymbol( int simpleSymbolHeight ) :
	exponentLine( simpleSymbolHeight * 0.4 )
	, radicandLine( simpleSymbolHeight * 0.8 )
{}

CSymbol* CRootSymbol::Clone( CLineOfSymbols* parent ) const
{
	CSymbol* copy = new CRootSymbol( *this );
	copy->UpdateParent( parent );
	return copy;
}

void CRootSymbol::UpdateParent( CLineOfSymbols* parent )
{
	exponentLine.SetParent( parent );
	radicandLine.SetParent( parent );
}


void CRootSymbol::GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const
{
	substrings.push_back( &exponentLine );
	substrings.push_back( &radicandLine );
}

void CRootSymbol::GetSubstrings( std::vector<CLineOfSymbols*>& substrings )
{
	substrings.push_back( &exponentLine );
	substrings.push_back( &radicandLine );
}

void CRootSymbol::Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const
{
	int rootHeight = radicandLine.GetHeight() / 0.8;
	//Рисуем экспоненту
	int exponentY = posY + rootHeight * 0.6 - exponentLine.GetHeight();
	exponentLine.Draw( displayHandle, posX, exponentY );
	int exponentWidth = exponentLine.CalculateWidth( displayHandle );

	//Рисуем знак корня
	//Устанавливаем шрифт для корня
	HFONT oldFont = static_cast<HFONT>( ::GetCurrentObject( displayHandle, OBJ_FONT ) );
	assert( oldFont != 0 );

	LOGFONT fontInfo;
	::GetObject( oldFont, sizeof( LOGFONT ), &fontInfo );
	fontInfo.lfHeight = rootHeight;
	HFONT font = ::CreateFontIndirect( &fontInfo );
	assert( font != 0 );
	oldFont = static_cast<HFONT>( ::SelectObject( displayHandle, font ) );

	SIZE fontSizeStruct;
	::GetTextExtentPoint32( displayHandle, symbol, 1, &fontSizeStruct );

	TEXTMETRIC textMetric;
	::GetTextMetrics( displayHandle, &textMetric );

	//Ширина символа корня
	int rootWidth = fontSizeStruct.cx;

	//Вычисляем координаты для сигмы и рисуем её
	::TextOut( displayHandle, posX + exponentWidth, posY, symbol, 1 );

	//Возвращаем старый шрифт, удаляем созданный
	::SelectObject( displayHandle, oldFont );
	::DeleteObject( font );

	//Рисуем подкоренное выражение

	int radicandX = posX + exponentWidth + rootWidth;
	int radicandY = posY + rootHeight / 5;
	radicandLine.Draw( displayHandle, radicandX, radicandY );
	int radicandWidth = radicandLine.CalculateWidth( displayHandle );

	//Рисуем линию
	int strokeStartX = radicandX;
	int strokeStartY = posY + textMetric.tmInternalLeading / 2;
	::MoveToEx( displayHandle, strokeStartX, strokeStartY, NULL );
	::LineTo( displayHandle, strokeStartX + radicandWidth, strokeStartY );

	//Обновляем информацию
	x = posX;
	y = posY - max( 0, exponentLine.GetHeight() - rootHeight * 0.4 );
	width = exponentWidth + rootWidth + radicandWidth;
	height = max( rootHeight, exponentLine.GetHeight() + rootHeight * 0.6 );

}

int CRootSymbol::CalculateWidth( HDC displayHandle ) const
{
	HFONT oldFont = static_cast<HFONT>( ::GetCurrentObject( displayHandle, OBJ_FONT ) );
	assert( oldFont != 0 );
	LOGFONT fontInfo;
	::GetObject( oldFont, sizeof( LOGFONT ), &fontInfo );
	fontInfo.lfHeight = getRootHeight( radicandLine.GetHeight() );
	HFONT font = ::CreateFontIndirect( &fontInfo );
	assert( font != 0 );
	oldFont = static_cast<HFONT>( ::SelectObject( displayHandle, font ) );

	SIZE fontSizeStruct;
	::GetTextExtentPoint32( displayHandle, symbol, 1, &fontSizeStruct );

	TEXTMETRIC textMetric;
	::GetTextMetrics( displayHandle, &textMetric );

	//Ширина символа корня
	int rootWidth = fontSizeStruct.cx;
	width = exponentLine.GetWidth() + rootWidth + radicandLine.GetWidth();

	return width;
}

int CRootSymbol::GetHeight( int simpleSymbolHeight ) const
{
	int rootHeight = radicandLine.GetHeight() / 0.8;
	height = max( rootHeight, exponentLine.GetHeight() + rootHeight * 0.6 );
	return height;
}

int CRootSymbol::GetBaselineOffset( int simpleSymbolHeight ) const
{
	return max( 0, exponentLine.GetHeight() - radicandLine.GetHeight() / 2 );
}

int CRootSymbol::GetDescent( int simpleSymbolHeight ) const
{
	return radicandLine.GetHeight() / 0.8;
}