// Автор: Воротилов Владислав

#include "FunctionSymbol.h"
#include "SimpleSymbol.h"
#include <assert.h>
#include <wingdi.h>
#include <Windows.h>

CFunctionSymbol::CFunctionSymbol( int simpleSymbolHeight, wchar_t* _funcName ) :
	argumentLine( simpleSymbolHeight ), functionName( simpleSymbolHeight ), openingBracket( simpleSymbolHeight), closingBracket( simpleSymbolHeight )
{
	// Заполняем имя функции
	int i = 0;
	while( _funcName[i] != 0 ) {
		functionName.PushBack( new CSimpleSymbol( _funcName[i] ) );
		i++;
	}
	openingBracket.PushBack( new CSimpleSymbol( L'(' ) );
	closingBracket.PushBack( new CSimpleSymbol( L')' ) );
}

CSymbol* CFunctionSymbol::Clone( CLineOfSymbols* parent ) const
{
	CSymbol* copy = new CFunctionSymbol( *this );
	copy->UpdateParent( parent );
	return copy;
}

void CFunctionSymbol::UpdateParent( CLineOfSymbols* parent )
{
	argumentLine.SetParent( parent );
}


void CFunctionSymbol::GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const
{
	substrings.push_back( &argumentLine );
}

void CFunctionSymbol::GetSubstrings( std::vector<CLineOfSymbols*>& substrings )
{
	substrings.push_back( &argumentLine );
}

void CFunctionSymbol::Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const
{
	int baselineOffset = max( functionName.getBaselineOffset(), argumentLine.getBaselineOffset() );
	// Рисуем название
	functionName.Draw( displayHandle, posX, posY + baselineOffset - GetBaselineOffset( simpleSymbolHeight ) );
	int functionNameWidth = functionName.CalculateWidth( displayHandle );
	// Рисуем открывающуюся скобку.
	openingBracket.Draw( displayHandle, posX + functionNameWidth, posY + baselineOffset - GetBaselineOffset( simpleSymbolHeight ) );
	int openingBracketWidth = openingBracket.CalculateWidth( displayHandle );

	//Рисуем аргумент
	argumentLine.Draw( displayHandle, posX + functionNameWidth + openingBracketWidth, posY - GetBaselineOffset( simpleSymbolHeight ) );
	int argumentWidth = argumentLine.CalculateWidth( displayHandle );

	// Рисуем закрывающуюся скобку
	closingBracket.Draw( displayHandle, posX + functionNameWidth + argumentWidth + openingBracketWidth, posY + baselineOffset - GetBaselineOffset( simpleSymbolHeight ) );
	int closingBracketWidth = closingBracket.CalculateWidth( displayHandle );

	//Обновляем информацию
	x = posX;
	y = posY;
	width =  argumentWidth + functionNameWidth + closingBracketWidth + openingBracketWidth;
	height = simpleSymbolHeight;
}

int CFunctionSymbol::CalculateWidth( HDC displayHandle ) const
{
	// Ширина названия функции
	int functionNameWidth = functionName.CalculateWidth( displayHandle );
	// Ширина открывающейся скобки.
	int openingBracketWidth = openingBracket.CalculateWidth( displayHandle );
	// Ширина аргумента функции
	int argumentWidth = argumentLine.CalculateWidth( displayHandle );
	// Ширина закрывающейся скобки
	int closingBracketWidth = closingBracket.CalculateWidth( displayHandle );
	width = functionNameWidth + openingBracketWidth + argumentWidth + closingBracketWidth;
	return width;
}

int CFunctionSymbol::GetHeight( int simpleSymbolHeight ) const
{
	return max( functionName.GetHeight(), argumentLine.GetHeight() );
}

int CFunctionSymbol::GetBaselineOffset( int simpleSymbolHeight ) const
{
	return max( functionName.getBaselineOffset(), argumentLine.getBaselineOffset() );
}

int CFunctionSymbol::GetDescent( int simpleSymbolHeight ) const
{
	return max( functionName.GetHeight() - functionName.getBaselineOffset(), argumentLine.GetHeight() - argumentLine.getBaselineOffset() );
}

// Преобразование символа в необходимый плоттеру формат.
std::string CFunctionSymbol::ToPlotterString() const
{
	return functionName.ToPlotterString() + "(" + argumentLine.ToPlotterString() + ")";
}

// Преобразование символа в Latex формат.
std::string CFunctionSymbol::ToLatexString() const
{
	return "\\" + functionName.ToLatexString() + "(" + argumentLine.ToLatexString() + ")";
}