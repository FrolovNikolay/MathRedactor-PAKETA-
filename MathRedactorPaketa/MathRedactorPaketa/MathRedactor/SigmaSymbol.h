//Автор: Давлетшин Азат

#pragma once

#include "Symbol.h"
#include "LineOfSymbols.h" 

//Сложный символ "сигма". Состоит из двух строк (верхней и нижней)
class CSigmaSymbol : public CSymbol {
public:

	CSigmaSymbol( int simpleSymbolHeight );

	//Создание копии (deep) объекта. 
	virtual CSymbol* Clone( CLineOfSymbols* parent ) const;
	virtual void UpdateParent( CLineOfSymbols* parent );
	void GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const;
	void GetSubstrings( std::vector<CLineOfSymbols*>& substrings );

	virtual int GetHeight( int simpleSymbolHeight ) const;
	virtual int GetBaselineOffset( int simpleSymbolHeight ) const;
	virtual int GetDescent( int simpleSymbolHeight ) const;

	CLineOfSymbols& GetUpperLine() { return upperLine; }
	const CLineOfSymbols& GetUpperLine() const { return upperLine; }
	CLineOfSymbols& GetLowerLine() { return lowerLine; }
	const CLineOfSymbols& GetLowerLine() const { return lowerLine; }

	//Описание этих методов находится в Symbol.h
	virtual void Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const;
	virtual int CalculateWidth( HDC displayHandle ) const;

private:
	//Строки над сигмой и под сигмой
	CLineOfSymbols upperLine;
	CLineOfSymbols lowerLine;

	//Символ сигмы для TextOut
	static const wchar_t symbol[];

	//Рассчитать высоту нижней и верхней строки
	static int calculateSublineHeight( int simpleSymbolHeight ) 
		{ return static_cast<int>( simpleSymbolHeight * 0.7 ); }
	//Высота символа сигмы
	static int getSigmaHeight( int simpleSymbolHeight ) 
		{ return static_cast<int>( simpleSymbolHeight * 1.5 ); }
	//Сдвиг верхней и нижней строк относительно сигмы
	static int getSublinesOffset() { return 1; }
};