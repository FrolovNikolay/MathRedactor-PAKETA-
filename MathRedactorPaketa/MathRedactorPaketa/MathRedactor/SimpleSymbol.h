//Автор: Давлетшин Азат

#pragma once

#include "Symbol.h"
#include "LineOfSymbols.h"

//Простые символы перекрываются, если не добавить этот сдвиг (прибавляется к ширине символа)
static const int SideOffset = 2;

//Простой символ (буква, цифра или простой оператор). Нет конструктора по-умолчанию.
class CSimpleSymbol : public CSymbol {
public:
	CSimpleSymbol( wchar_t _symbol ) { symbol = _symbol; }

	//Получить высоту символа
	virtual int GetHeight( int simpleSymbolHeight ) const { return simpleSymbolHeight; }
	virtual int GetBaselineOffset( int simpleSymbolHeight ) const { return 0; }
	virtual int GetDescent( int simpleSymbolHeight ) const { return simpleSymbolHeight; }

	//Создание копии (deep) объекта
	virtual CSymbol* Clone( CLineOfSymbols* parent ) const { return new CSimpleSymbol( *this ); }
	virtual void UpdateParent( CLineOfSymbols* parent ) {}
	//У простого символа нет подстрок
	virtual void GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const {}
	virtual void GetSubstrings( std::vector<CLineOfSymbols*>& substrings ) {}

	wchar_t GetSymbol( ) const { return symbol; }
	void SetSymbol( wchar_t _symbol ) { symbol = _symbol; }

	//Описание этих методов находится в Symbol.h
	virtual void Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const;
	virtual int CalculateWidth( HDC displayHandle ) const;

private:
	//Имя символа (передается в TextOut).
	wchar_t symbol;
};