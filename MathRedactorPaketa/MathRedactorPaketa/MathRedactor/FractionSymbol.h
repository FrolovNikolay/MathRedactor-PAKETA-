//Автор: Давлетшин Азат

#pragma once

#include "Symbol.h"
#include "LineOfSymbols.h" 

//Сложный символ "дробь". Состоит из двух строк (LineOfSymbols).
class CFractionSymbol : public CSymbol {
public:

	CFractionSymbol( int simpleSymbolHeight );

	//Создание копии (deep) объекта. 
	virtual CSymbol* Clone( CLineOfSymbols* ) const;
	virtual void UpdateParent( CLineOfSymbols* parent );
	//Получить массив указателей на подстроки (может быть пустым)
	virtual void GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const;
	virtual void GetSubstrings( std::vector<CLineOfSymbols*>& substrings );

	virtual int GetHeight( int simpleSymbolHeight ) const;
	virtual int GetBaselineOffset( int simpleSymbolHeight ) const;
	virtual int GetDescent( int simpleSymbolHeight ) const;

	CLineOfSymbols& GetUpperLine( ) { return upperLine; }
	const CLineOfSymbols& GetUpperLine( ) const { return upperLine; }
	CLineOfSymbols& GetLowerLine( ) { return lowerLine; }
	const CLineOfSymbols& GetLowerLine( ) const { return lowerLine; }

	//Описание этих методов находится в Symbol.h
	virtual void Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const;
	virtual int CalculateWidth( HDC displayHandle ) const;

private:
	//Верхняя и нижняя части дроби
	CLineOfSymbols upperLine;
	CLineOfSymbols lowerLine;

	//Методы определяют сдвиги относительно линии дроби и толщину самой линии
	int getUpperOffset( ) const { return 3; }
	int getLowerOffset( ) const { return 3; }
	int getStrokeHeight( ) const { return 1; }
	//Добавка к ширине линии (добавляется с каждой стороны)
	int getStrokeAddition( ) const { return 1; }
	int getSymbolBorder( ) const { return 1; }
};