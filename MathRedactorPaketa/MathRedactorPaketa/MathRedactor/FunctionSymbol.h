// Автор: Воротилов Владислав
// Описание: Класс представляет любые функции с одним аргументом
// в том числе синус, косинус и тд

#pragma once

#include "Symbol.h"
#include "LineOfSymbols.h" 

class CFunctionSymbol : public CSymbol {
public:

	CFunctionSymbol( int simpleSymbolHeight, wchar_t* _funcName );

	// Создание копии (deep) объекта. 
	virtual CSymbol* Clone( CLineOfSymbols* parent ) const;
	virtual void UpdateParent( CLineOfSymbols* parent );

	// Получить массив указателей на подстроки (может быть пустым) 
	virtual void GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const;
	virtual void GetSubstrings( std::vector<CLineOfSymbols*>& substrings );

	virtual int GetHeight( int simpleSymbolHeight ) const;
	virtual int GetBaselineOffset( int simpleSymbolHeight ) const;
	virtual int GetDescent( int simpleSymbolHeight ) const;

	CLineOfSymbols& GetArgumentLine() { return argumentLine; }
	const CLineOfSymbols& GetArgumentLine() const { return argumentLine; }

	// Описание этих методов находится в Symbol.h
	virtual void Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const;
	virtual int CalculateWidth( HDC displayHandle ) const;

	// Преобразование символа в необходимый плоттеру формат.
	virtual std::string ToPlotterString() const;
	// Преобразование символа в Latex формат.
	virtual std::string ToLatexString() const;

private:

	// Название функции которое отображается на экране
	CLineOfSymbols functionName;
	// Закрывающаяся круглая скобка идущая последним отображенным символом
	CLineOfSymbols openingBracket;
	// Аргументы функции
	CLineOfSymbols argumentLine;
	// Закрывающаяся круглая скобка идущая последним отображенным символом
	CLineOfSymbols closingBracket;

};