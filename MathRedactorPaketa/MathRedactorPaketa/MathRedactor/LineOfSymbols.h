//Автор: Давлетшин Азат
//Описание: Здесь представлен класс строки, который хранит в себе массив символов.

#pragma once

#include <Windows.h>
#include <vector>
#include "Symbol.h"

//Класс представляет собой строку символов. Умеет отрисовывать себя в hdc.
class CLineOfSymbols {
public:
	CLineOfSymbols( int _simpleSymbolHeight );
	CLineOfSymbols( const CLineOfSymbols& src );
	~CLineOfSymbols();

	CLineOfSymbols& operator=( const CLineOfSymbols& src );

	int GetX() const { return x; }
	int GetY() const { return y; }
	int GetWidth() const { return width; }
	int GetHeight( ) const { return height; }
	int GetSimpleSymbolHeight( ) const { return simpleSymbolHeight; }

	//Родителя устанавливает тот, кто создает строку
	void SetParent( CLineOfSymbols* _parent ) { parent = _parent; }

	//Нарисовать строку в Device Context, начиная с позиции (startX, startY) - левый верхний угол строки.
	void Draw( HDC displayHandle, int posX, int posY ) const;
	//Рассчитать ширину строки (линейно по количеству символов в строке, включая символы в сложных символах)
	int CalculateWidth( HDC displayHandle ) const;
	//Пересчитать высоты и базовые линии
	void Recalculate( );

	// Добавление символа в конец
	void PushBack( CSymbol* symbol ) {
		Push( symbol, Length() );
	}

	// Добавление символа в позицию index
	// Если index лежит за пределами строки то добавляет в конец строки
	void Push( CSymbol* symbol, int index );
	//Убирает символ с текущей позиции
	void Pop( int index );
	//Возвращает длину строки
	int Length( ) const { return static_cast<int>(arrayOfSymbolPtrs.size( )); }

	const CSymbol* operator[] ( int index ) const { return arrayOfSymbolPtrs[index]; }
	CSymbol* operator[] ( int index ) { return arrayOfSymbolPtrs[index]; }



private:
	std::vector<CSymbol*> arrayOfSymbolPtrs;
	//Сдвиг базовой линии относительно верха строки. Служит для позиционирования символов (верхняя точка простых символов).
	int baselineOffset;
	//Размер (высота) базового символа. Меняется только специальным методом.
	int simpleSymbolHeight;
	//Если строка находится в сложном символе, то родитель - это строка в которой находится сложный символ
	CLineOfSymbols* parent;

	//Левый верхний угол, ширина, высота (обновляется при перерисовке)
	//Служебная инфа, поэтому mutable (символ сам от этого не меняется)
	mutable int x;
	mutable int y;
	mutable int width;
	mutable int height;
};