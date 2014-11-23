#include "RootSymbol.h"
#include <assert.h>
#include <wingdi.h>
#include <Windows.h>

const wchar_t CRootSymbol::symbol[] = { L'\u221A' };

CRootSymbol::CRootSymbol( int simpleSymbolHeight ) :
	exponentLine( exponentSymbolSize( simpleSymbolHeight ) )
	, radicandLine( radicandSymbolSize( simpleSymbolHeight ) )
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
	int rootHeight = rootSymbolHeight( radicandLine.GetHeight() );
	//Рисуем экспоненту
	int exponentY = posY + exponentOffset( rootHeight ) - radicandLine.getBaselineOffset() - exponentLine.GetHeight() + baseOffset();
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
	::TextOut( displayHandle, posX + exponentWidth, posY - rootOffset( rootHeight ) - radicandLine.getBaselineOffset() + baseOffset(), symbol, 1 );
	//Возвращаем старый шрифт, удаляем созданный
	::SelectObject( displayHandle, oldFont );
	::DeleteObject( font );

	//Рисуем подкоренное выражение

	int radicandX = posX + exponentWidth + rootWidth;
	int radicandY = posY - radicandLine.getBaselineOffset() + baseOffset();
	radicandLine.Draw( displayHandle, radicandX, radicandY );
	int radicandWidth = radicandLine.CalculateWidth( displayHandle );

	//Рисуем линию
	int strokeStartX = radicandX;
	int strokeStartY = posY - rootOffset( rootHeight ) +  baseOffset()  - radicandLine.getBaselineOffset() + textMetric.tmInternalLeading / 2;
	::MoveToEx( displayHandle, strokeStartX, strokeStartY, NULL );
	::LineTo( displayHandle, strokeStartX + radicandWidth, strokeStartY );

	//Обновляем информацию
	x = posX;
	y = posY - max( rootSymbolHeight( rootHeight ), exponentLine.GetHeight() - exponentOffset( rootHeight ) );
	width = exponentWidth + rootWidth + radicandWidth;
	height = max( rootHeight, exponentLine.GetHeight() + ( rootHeight - exponentOffset( rootHeight ) ) ) + baseOffset();

}

int CRootSymbol::CalculateWidth( HDC displayHandle ) const
{
	HFONT oldFont = static_cast<HFONT>( ::GetCurrentObject( displayHandle, OBJ_FONT ) );
	assert( oldFont != 0 );
	LOGFONT fontInfo;
	::GetObject( oldFont, sizeof( LOGFONT ), &fontInfo );
	fontInfo.lfHeight = rootSymbolHeight( radicandLine.GetHeight() );
	HFONT font = ::CreateFontIndirect( &fontInfo );
	assert( font != 0 );
	oldFont = static_cast<HFONT>( ::SelectObject( displayHandle, font ) );

	SIZE fontSizeStruct;
	::GetTextExtentPoint32( displayHandle, symbol, 1, &fontSizeStruct );

	TEXTMETRIC textMetric;
	::GetTextMetrics( displayHandle, &textMetric );

	//Ширина символа корня
	int rootWidth = fontSizeStruct.cx;
	width = exponentLine.CalculateWidth( displayHandle ) + rootWidth + radicandLine.CalculateWidth( displayHandle );

	::SelectObject( displayHandle, oldFont );
	::DeleteObject( font );
	return width;
}

int CRootSymbol::GetHeight( int simpleSymbolHeight ) const
{
	int rootHeight = rootSymbolHeight( radicandLine.GetHeight() );
	height = max( rootHeight, exponentLine.GetHeight() + ( rootHeight - exponentOffset( rootHeight ) - rootOffset( rootHeight ) ) ) + baseOffset();
	return height;
}

int CRootSymbol::GetBaselineOffset( int simpleSymbolHeight ) const
{
	int rootHeight = rootSymbolHeight( radicandLine.GetHeight() );
	return max( exponentLine.GetHeight() - exponentOffset( rootHeight ) + radicandLine.getBaselineOffset(), rootOffset( rootHeight ) + radicandLine.getBaselineOffset() );
}

int CRootSymbol::GetDescent( int simpleSymbolHeight ) const
{
	return radicandLine.GetHeight() - radicandLine.getBaselineOffset() + baseOffset();
}

// Преобразование символа, в необходимый плоттеру формат.
std::string CRootSymbol::ToPlotterString() const
{
	if( exponentLine.Length() != 0 ) {
		return "(" + radicandLine.ToPlotterString() + ")^(1/" + exponentLine.ToPlotterString() + ")"; 
	} else {
		return "(" + radicandLine.ToPlotterString() + ")^(1/2)"; 
	}
}
