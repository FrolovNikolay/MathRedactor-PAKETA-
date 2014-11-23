// Автор: Давлетшин Азат.

// Описание: Сложный символ "корень". Состоит из двух строк (степень и подкоренное выражение)

#pragma once

#include "Symbol.h"
#include "LineOfSymbols.h" 

class CRootSymbol : public CSymbol {
public:

	CRootSymbol( int simpleSymbolHeight );

	// Создание копии (deep) объекта. 
	virtual CSymbol* Clone( CLineOfSymbols* parent ) const;
	virtual void UpdateParent( CLineOfSymbols* parent );
	// Получить массив указателей на подстроки (может быть пустым).
	virtual void GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const;
	virtual void GetSubstrings( std::vector<CLineOfSymbols*>& substrings );

	virtual int GetHeight( int simpleSymbolHeight ) const;
	virtual int GetBaselineOffset( int simpleSymbolHeight ) const;
	virtual int GetDescent( int simpleSymbolHeight ) const;

	CLineOfSymbols& GetExponentLine( ) { return exponentLine; }
	const CLineOfSymbols& GetExponentLine( ) const { return exponentLine; }
	CLineOfSymbols& GetRadicandLine( ) { return radicandLine; }
	const CLineOfSymbols& GetRadicandLine( ) const { return radicandLine; }

	// Описание этих методов находится в Symbol.h.
	virtual void Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const;
	virtual int CalculateWidth( HDC displayHandle ) const;

	// Преобразование символа в необходимый плоттеру формат.
	virtual std::string ToPlotterString() const;
	// Преобразование символа в Latex формат.
	virtual std::string ToLatexString() const;

private:
	// Строки степень и подкоренное выражение.
	CLineOfSymbols exponentLine;
	CLineOfSymbols radicandLine;

	// Символ корня для TextOut.
	static const wchar_t symbol[];

	static int radicandSymbolSize( int symbolSize ) { return static_cast<int>( symbolSize * 0.8 ); }
	// Рассчитать высоту строки степени.
	static int exponentSymbolSize( int symbolSize ) { return static_cast<int>( symbolSize * 0.4 ); }
	// Высота юникодного символа корня.
	static int rootSymbolHeight( int symbolSize ) { return static_cast<int>( symbolSize / 0.8 ); }

	static int exponentOffset( int rootHeight ) { return static_cast<int>( rootHeight * 0.2 ); }

	static int radicantOffset( int rootHeight ) { return static_cast<int>( rootHeight * 0.2 ); }

	static int rootOffset( int rootHeight ) { return static_cast<int>( rootHeight * 0.2 ); }

	static int baseOffset() { return 10; }
};