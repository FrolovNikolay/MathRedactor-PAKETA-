#include "FractionSymbol.h"
#include <assert.h>
#include <wingdi.h>
#include <Windows.h>

CFractionSymbol::CFractionSymbol( int simpleSymbolHeight ) :
upperLine( simpleSymbolHeight ),
lowerLine( simpleSymbolHeight )
{

}

CSymbol* CFractionSymbol::Clone( CLineOfSymbols* parent ) const
{
	CSymbol* copy = new CFractionSymbol( *this );
	copy->UpdateParent( parent );
	return copy;
}

void CFractionSymbol::UpdateParent( CLineOfSymbols* parent )
{
	upperLine.SetParent( parent );
	lowerLine.SetParent( parent );
}


void CFractionSymbol::GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const
{
	substrings.push_back( &upperLine );
	substrings.push_back( &lowerLine );
}

void CFractionSymbol::GetSubstrings( std::vector<CLineOfSymbols*>& substrings )
{
	substrings.push_back( &upperLine );
	substrings.push_back( &lowerLine );
}


void CFractionSymbol::Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const
{
	//Рисуем разделитель
	int strokeStartY = posY + simpleSymbolHeight / 2;
	::MoveToEx( displayHandle, posX + getSymbolBorder( ), strokeStartY, NULL );
	int upperWidth = upperLine.CalculateWidth( displayHandle );
	int lowerWidth = lowerLine.CalculateWidth( displayHandle );
	int maxWidth = max( upperWidth, lowerWidth );
	int strokeWidth = maxWidth + 2 * getStrokeAddition( );
	::LineTo( displayHandle, posX + getSymbolBorder( ) + strokeWidth, strokeStartY );

	posX += getStrokeAddition( ) + getSymbolBorder( );

	//Рисуем верхнюю линию
	int upperStartX = posX + (maxWidth - upperWidth) / 2;
	int upperStartY = strokeStartY - getUpperOffset( ) - upperLine.GetHeight( );
	upperLine.Draw( displayHandle, upperStartX, upperStartY );

	//Рисуем нижнюю линию
	int lowerStartX = posX + (maxWidth - lowerWidth) / 2;
	int lowerStartY = strokeStartY + getStrokeHeight( ) + getLowerOffset( );
	lowerLine.Draw( displayHandle, lowerStartX, lowerStartY );

	//Обновление служебной информации
	x = posX - getStrokeAddition() - getSymbolBorder();
	y = upperStartY;
	width = strokeWidth + 2 * getSymbolBorder( );
	height = upperLine.GetHeight( ) + lowerLine.GetHeight( ) + getUpperOffset( ) +
		getLowerOffset( ) + getStrokeHeight( );

}

int CFractionSymbol::CalculateWidth( HDC displayHandle ) const
{
	int upperWidth = upperLine.CalculateWidth( displayHandle );
	int lowerWidth = lowerLine.CalculateWidth( displayHandle );

	width = max( upperWidth, lowerWidth ) + 2 * getStrokeAddition( ) + 2 * getSymbolBorder( );

	return width;
}

int CFractionSymbol::GetHeight( int simpleSymbolHeight ) const
{
	return upperLine.GetHeight() + lowerLine.GetHeight() + getUpperOffset() +
		getLowerOffset() + getStrokeHeight();
}

int CFractionSymbol::GetBaselineOffset( int simpleSymbolHeight ) const
{
	return upperLine.GetHeight() + getUpperOffset() - simpleSymbolHeight / 2;
}

int CFractionSymbol::GetDescent( int simpleSymbolHeight ) const
{
	return lowerLine.GetHeight() + getLowerOffset() + getStrokeHeight() + simpleSymbolHeight / 2;
}