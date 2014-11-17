//Автор: Давлетшин Азат

#pragma once

#include "Symbol.h"
#include "LineOfSymbols.h" 

//Сложный символ "корень". Состоит из двух строк (степень и подкоренное выражение)
class CRootSymbol : public CSymbol {
public:

	CRootSymbol( int simpleSymbolHeight );

	//Создание копии (deep) объекта. 
	virtual CSymbol* Clone( CLineOfSymbols* parent ) const;
	virtual void UpdateParent( CLineOfSymbols* parent );
	//Получить массив указателей на подстроки (может быть пустым) 
	virtual void GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const;
	virtual void GetSubstrings( std::vector<CLineOfSymbols*>& substrings );

	virtual int GetHeight( int simpleSymbolHeight ) const;
	virtual int GetBaselineOffset( int simpleSymbolHeight ) const;
	virtual int GetDescent( int simpleSymbolHeight ) const;

	CLineOfSymbols& GetExponentLine( ) { return exponentLine; }
	const CLineOfSymbols& GetExponentLine( ) const { return exponentLine; }
	CLineOfSymbols& GetRadicandLine( ) { return radicandLine; }
	const CLineOfSymbols& GetRadicandLine( ) const { return radicandLine; }

	//Описание этих методов находится в Symbol.h
	virtual void Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const;
	virtual int CalculateWidth( HDC displayHandle ) const;

private:
	//Строки степень и подкоренное выражение
	CLineOfSymbols exponentLine;
	CLineOfSymbols radicandLine;

	//Символ корня для TextOut
	static const wchar_t symbol[];

	//Рассчитать высоту строки степени
	static int getExponentHeight( int simpleSymbolHeight )
		{ return static_cast<int>( simpleSymbolHeight * 0.5 ); }
	//Высота юникодного символа корня
	static int getRootHeight( int simpleSymbolHeight )
	{
		return static_cast<int>( simpleSymbolHeight * 1.6 );
	}
	//Сдвиг по горизонтали показателя степени
	static int getExponentOffset( int rootWidth )
	{
		return -static_cast<int>( rootWidth * 0.4 );
	}
	static int getExponentVerticalOffset( int rootHeight )
	{
		return static_cast<int>( rootHeight * 0.4 );
	}
};