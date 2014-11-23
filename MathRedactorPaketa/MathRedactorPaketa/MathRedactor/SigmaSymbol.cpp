#include "SigmaSymbol.h"
#include <assert.h>
#include <wingdi.h>
#include <Windows.h>


CSigmaSymbol::CSigmaSymbol( int simpleSymbolHeight, const wchar_t symbolForDawing ) :
	upperLine( calculateSublineHeight( simpleSymbolHeight ) ),
	lowerLine( calculateSublineHeight( simpleSymbolHeight ) )
{
	symbol[0] = symbolForDawing;
	symbol[1] = 0;
}

CSymbol* CSigmaSymbol::Clone( CLineOfSymbols* parent ) const
{
	CSymbol* copy = new CSigmaSymbol( *this );
	copy->UpdateParent( parent );
	return copy;
}

void CSigmaSymbol::UpdateParent( CLineOfSymbols* parent )
{
	upperLine.SetParent( parent );
	lowerLine.SetParent( parent );
}

void CSigmaSymbol::GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const
{
	substrings.push_back( &upperLine );
	substrings.push_back( &lowerLine );
}

void CSigmaSymbol::GetSubstrings( std::vector<CLineOfSymbols*>& substrings )
{
	substrings.push_back( &upperLine );
	substrings.push_back( &lowerLine );
}

void CSigmaSymbol::Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const
{
	int upperWidth = upperLine.CalculateWidth( displayHandle );
	int lowerWidth = lowerLine.CalculateWidth( displayHandle );
	width = max( upperWidth, lowerWidth );
	
	//Устанавливаем шрифт для сигмы
	HFONT oldFont = ( HFONT )::GetCurrentObject( displayHandle, OBJ_FONT );
	assert( oldFont != 0 );

	LOGFONT fontInfo;
	::GetObject( oldFont, sizeof( LOGFONT ), &fontInfo );
	fontInfo.lfHeight = getSigmaHeight( simpleSymbolHeight );
	HFONT font = ::CreateFontIndirect( &fontInfo );
	assert( font != 0 );
	oldFont = ( HFONT )::SelectObject( displayHandle, font );

	SIZE fontSizeStruct;
	::GetTextExtentPoint32( displayHandle, symbol, 1, &fontSizeStruct );

	//Ширина символа сигмы
	int sigmaWidth = fontSizeStruct.cx;
	width = max( width, sigmaWidth );

	//Вычисляем координаты для сигмы и рисуем её
	int sigmaX = ( width - sigmaWidth ) / 2 + posX;
	int sigmaY = posY - ( getSigmaHeight( simpleSymbolHeight ) - simpleSymbolHeight ) / 2;
	sigmaY -= getSublinesOffset();
	::TextOut( displayHandle, sigmaX, sigmaY, symbol, 1 );

	//Возвращаем старый шрифт, удаляем созданный
	::SelectObject( displayHandle, oldFont );
	::DeleteObject( font );

	//Рисуем верхнюю строку
	int upperX = posX + ( width - upperWidth ) / 2;
	int upperY = sigmaY - upperLine.GetHeight();
	upperLine.Draw( displayHandle, upperX, upperY );

	//Рисуем нижнюю строку
	int lowerX = posX + ( width - lowerWidth ) / 2;
	int lowerY = sigmaY + getSigmaHeight( simpleSymbolHeight ) + getSublinesOffset();
	lowerLine.Draw( displayHandle, lowerX, lowerY );

	x = posX;
	y = upperY;
	height = upperLine.GetHeight() + getSublinesOffset() + getSigmaHeight( simpleSymbolHeight ) +
		getSublinesOffset() + lowerLine.GetHeight();

}

int CSigmaSymbol::CalculateWidth( HDC displayHandle ) const
{
	int upperWidth = upperLine.CalculateWidth( displayHandle );
	int lowerWidth = lowerLine.CalculateWidth( displayHandle );

	width = max( upperWidth, lowerWidth );

	//Устанавливаем шрифт для сигмы
	HFONT oldFont = ( HFONT )::GetCurrentObject( displayHandle, OBJ_FONT );
	assert( oldFont != 0 );

	LOGFONT fontInfo;
	::GetObject( oldFont, sizeof( LOGFONT ), &fontInfo );
	fontInfo.lfHeight = getSigmaHeight( fontInfo.lfHeight );
	HFONT font = ::CreateFontIndirect( &fontInfo );
	assert( font != 0 );
	oldFont = ( HFONT )::SelectObject( displayHandle, font );

	SIZE fontSizeStruct;
	::GetTextExtentPoint32( displayHandle, symbol, 1, &fontSizeStruct );

	//Ширина символа сигмы
	int sigmaWidth = fontSizeStruct.cx;
	width = max( width, sigmaWidth );

	//Возвращаем старый шрифт, удаляем созданный
	::SelectObject( displayHandle, oldFont );
	::DeleteObject( font );

	return width;
}

int CSigmaSymbol::GetHeight( int simpleSymbolHeight ) const
{
	height = upperLine.GetHeight( ) + getSublinesOffset( ) + getSigmaHeight( simpleSymbolHeight ) +
		getSublinesOffset( ) + lowerLine.GetHeight( );
	return height;
}

int CSigmaSymbol::GetBaselineOffset( int simpleSymbolHeight ) const
{
	return upperLine.GetHeight() + getSublinesOffset() +
		( getSigmaHeight( simpleSymbolHeight ) - simpleSymbolHeight ) / 2;
}

int CSigmaSymbol::GetDescent( int simpleSymbolHeight ) const
{
	return lowerLine.GetHeight() + getSublinesOffset() + ( getSigmaHeight( simpleSymbolHeight ) + simpleSymbolHeight ) / 2;
}

// Преобразование символа, в необходимый плоттеру формат.
std::string CSigmaSymbol::ToPlotterString() const
{
	// TODO: Plotter не работает с аггрегирующими функциями.
	return "";
}