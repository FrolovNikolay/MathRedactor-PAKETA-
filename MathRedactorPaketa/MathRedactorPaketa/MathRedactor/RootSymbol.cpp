#include "RootSymbol.h"
#include <assert.h>
#include <wingdi.h>
#include <Windows.h>

const wchar_t CRootSymbol::symbol[] = { L'\u221A' };

CRootSymbol::CRootSymbol( int simpleSymbolHeight ) :
	exponentLine( getExponentHeight( simpleSymbolHeight ) ),
	radicandLine( simpleSymbolHeight )
{

}

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
	int rootHeight = getRootHeight( simpleSymbolHeight );
	//Рисуем экспоненту
	int exponentY = posY - ( ( rootHeight - simpleSymbolHeight ) +
		( exponentLine.GetHeight() - getExponentVerticalOffset( rootHeight ) ) );
	exponentLine.Draw( displayHandle, posX, exponentY );
	int exponentWidth = exponentLine.CalculateWidth( displayHandle );

	//Рисуем знак корня
	//Устанавливаем шрифт для корня
	HFONT oldFont = ( HFONT )::GetCurrentObject( displayHandle, OBJ_FONT );
	assert( oldFont != 0 );

	LOGFONT fontInfo;
	::GetObject( oldFont, sizeof( LOGFONT ), &fontInfo );
	fontInfo.lfHeight = getRootHeight( simpleSymbolHeight );
	HFONT font = ::CreateFontIndirect( &fontInfo );
	assert( font != 0 );
	oldFont = ( HFONT )::SelectObject( displayHandle, font );

	SIZE fontSizeStruct;
	::GetTextExtentPoint32( displayHandle, symbol, 1, &fontSizeStruct );

	TEXTMETRIC textMetric;
	::GetTextMetrics( displayHandle, &textMetric );

	//Ширина символа корня
	int rootWidth = fontSizeStruct.cx;

	//Вычисляем координаты для сигмы и рисуем её
	int rootX = posX + exponentWidth + getExponentOffset( rootWidth );
	rootX = max( posX, rootX );
	int rootY = posY - ( rootHeight - simpleSymbolHeight );
	::TextOut( displayHandle, rootX, rootY, symbol, 1 );

	//Возвращаем старый шрифт, удаляем созданный
	::SelectObject( displayHandle, oldFont );
	::DeleteObject( font );

	//Рисуем подкоренное выражение
	int radicandX = rootX + rootWidth;
	int radicandY = posY;
	radicandLine.Draw( displayHandle, radicandX, radicandY );
	int radicandWidth = radicandLine.CalculateWidth( displayHandle );

	//Рисуем линию
	int strokeStartX = radicandX;
	int strokeStartY = rootY + textMetric.tmInternalLeading / 2;
	::MoveToEx( displayHandle, strokeStartX, strokeStartY, NULL );
	::LineTo( displayHandle, strokeStartX + radicandWidth, strokeStartY );

	//Обновляем информацию
	x = posX;
	y = exponentY;
	width = exponentWidth + rootWidth + getExponentOffset( rootWidth ) + radicandWidth;
	height = getRootHeight( simpleSymbolHeight ) + 
		( exponentLine.GetHeight( ) - getExponentVerticalOffset( rootHeight ) );

}

int CRootSymbol::CalculateWidth( HDC displayHandle ) const
{
	int exponentWidth = exponentLine.CalculateWidth( displayHandle );
	int radicandWidth = radicandLine.CalculateWidth( displayHandle );

	HFONT oldFont = ( HFONT )::GetCurrentObject( displayHandle, OBJ_FONT );
	assert( oldFont != 0 );

	LOGFONT fontInfo;
	::GetObject( oldFont, sizeof( LOGFONT ), &fontInfo );
	fontInfo.lfHeight = getRootHeight( fontInfo.lfHeight );
	HFONT font = ::CreateFontIndirect( &fontInfo );
	assert( font != 0 );
	oldFont = ( HFONT )::SelectObject( displayHandle, font );

	SIZE fontSizeStruct;
	::GetTextExtentPoint32( displayHandle, symbol, 1, &fontSizeStruct );

	//Ширина символа корня
	int rootWidth = fontSizeStruct.cx;

	//Возвращаем старый шрифт, удаляем созданный
	::SelectObject( displayHandle, oldFont );
	::DeleteObject( font );

	width = exponentWidth + rootWidth + getExponentOffset( rootWidth ) + radicandWidth;

	return width;
}

int CRootSymbol::GetHeight( int simpleSymbolHeight ) const
{
	int rootHeight = getRootHeight( simpleSymbolHeight );
	height = getRootHeight( simpleSymbolHeight ) +
		( exponentLine.GetHeight() - getExponentVerticalOffset( rootHeight ) );
	return height;
}

int CRootSymbol::GetBaselineOffset( int simpleSymbolHeight ) const
{
	int rootHeight = getRootHeight( simpleSymbolHeight );
	return ( exponentLine.GetHeight() - getExponentVerticalOffset( rootHeight ) ) +
		( rootHeight - simpleSymbolHeight );
}

int CRootSymbol::GetDescent( int simpleSymbolHeight ) const
{
	return simpleSymbolHeight;
}