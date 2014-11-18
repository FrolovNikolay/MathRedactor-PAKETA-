// Автор: Федюнин Валерий

#include "IndexSymbol.h"

#include <assert.h>

const float CIndexSymbol::heightRatio = 1.4F;
const float CIndexSymbol::sublineHeightRatio = 0.4F;

CIndexSymbol::CIndexSymbol( int simpleSymbolHeight, CIndexSymbol::TIndexType _type ) :
		type( _type ), line( calculateSublineHeight( simpleSymbolHeight ) )
{

}

void CIndexSymbol::UpdateParent( CLineOfSymbols* parent )
{
	line.SetParent( parent );
}

CSymbol* CIndexSymbol::Clone( CLineOfSymbols* parent ) const
{
	CSymbol* copy = new CIndexSymbol( *this );
	copy->UpdateParent( parent );
	return copy;
}


int CIndexSymbol::GetHeight( int simpleSymbolHeight ) const
{
	return line.GetHeight() + simpleSymbolHeight;
}

int CIndexSymbol::GetBaselineOffset( int simpleSymbolHeight ) const
{
	return line.GetHeight() / 2;
}

int CIndexSymbol::GetDescent( int simpleSymbolHeight ) const
{
	//TODO: подумать правильно ли это
	return simpleSymbolHeight;
}

CLineOfSymbols& CIndexSymbol::GetLine()
{
	return line;
}

const CLineOfSymbols& CIndexSymbol::GetLine() const
{
	return line;
}

void CIndexSymbol::Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const
{
	int lineWidth = line.CalculateWidth( displayHandle );

	switch( type ) {
	case ITUpper:
		line.Draw( displayHandle, posX, posY - ( line.GetHeight() / 2 ) );
		break;
	case ITLower:
		line.Draw( displayHandle, posX, posY + simpleSymbolHeight - ( line.GetHeight() / 2 ) );
		break;
	default:
		assert( false );
	}
	
	x = posX;
	y = posY - line.GetHeight() / 2;
	height = line.GetHeight() + simpleSymbolHeight;
	width = lineWidth;
}

int CIndexSymbol::CalculateWidth( HDC displayHandle ) const
{
	return line.CalculateWidth( displayHandle );
}

int CIndexSymbol::calculateSublineHeight( int simpleSymbolHeight )
{
	return static_cast< int >( simpleSymbolHeight * sublineHeightRatio );
}