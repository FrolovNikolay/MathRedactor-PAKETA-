// Автор: Давлетшин Азат.

// Описание: Здесь представлен класс строки, который хранит в себе массив символов.

#pragma once

#include <Windows.h>
#include <vector>
#include "Symbol.h"
#include <MathFormObj.h>


// Класс представляет собой строку символов. Умеет отрисовывать себя в hdc.
class CLineOfSymbols {
public:
	// isBase это флаг показывающий, является ли строка строкой редактора( то есть одной из базовых ), или подстрокой символа.
	CLineOfSymbols( int _simpleSymbolHeight, bool _isBase = false );
	CLineOfSymbols( const CLineOfSymbols& src );
    CLineOfSymbols( int _simpleSymbolHeight, const std::shared_ptr<MathObj> tree );
	~CLineOfSymbols();

	CLineOfSymbols& operator=( const CLineOfSymbols& src );

	int GetX() const { return x; }
	int GetY() const { return y; }
	int GetWidth() const { return width; }
	int GetHeight( ) const { return height; }
	int GetSimpleSymbolHeight( ) const { return simpleSymbolHeight; }

	// Родителя устанавливает тот, кто создает строку.
	void SetParent( CLineOfSymbols* _parent ) { parent = _parent; }

    // Получить родителя
    const CLineOfSymbols* GetParent() const { return parent; }

	// Нарисовать строку в Device Context, начиная с позиции (startX, startY) - левый верхний угол строки.
	void Draw( HDC displayHandle, int posX, int posY ) const;
	// Рассчитать ширину строки (линейно по количеству символов в строке, включая символы в сложных символах).
	int CalculateWidth( HDC displayHandle ) const;
	// Пересчитать высоты и базовые линии.
	void Recalculate();
	int getBaselineOffset() const { return baselineOffset; }
	// Добавление символа в конец.
	void PushBack( CSymbol* symbol ) { arrayOfSymbolPtrs.push_back( symbol ); }

	// Добавление символа в позицию index.
	void Insert( int index, CSymbol* symbol );

    // Конкатенация строк
    void Concatenate( CLineOfSymbols* line );

	// Убирает символ с текущей позиции.
	void Delete( int index );

    // Очищает строку
    void Clear() { arrayOfSymbolPtrs.clear(); }

	// Возвращает длину строки.
	int Length() const { return static_cast<int>( arrayOfSymbolPtrs.size() ); }

	// Является ли строка, строкой редактора(true) или сложного символа(false).
	bool IsBase() const { return isBase; }

	// Преобразование строки в необходимый плоттеру формат.
	std::string ToPlotterString( int firstEnableSymbol = 0 ) const;
	// Преобразование строки в Latex формат.
	std::string ToLatexString( int firstEnableSymbol = 0 ) const;

	const CSymbol* operator[] ( int index ) const { return arrayOfSymbolPtrs[index]; }
	CSymbol* operator[] ( int index ) { return arrayOfSymbolPtrs[index]; }

private:
	std::vector<CSymbol*> arrayOfSymbolPtrs;
	// Сдвиг базовой линии относительно верха строки. Служит для позиционирования символов (верхняя точка простых символов).
	int baselineOffset;
	// Размер (высота) базового символа. Меняется только специальным методом.
	int simpleSymbolHeight;
	// Если строка находится в сложном символе, то родитель - это строка в которой находится сложный символ.
	CLineOfSymbols* parent;
	// Является ли строка базовой(true) или подстрокой сложного символа(false).
	bool isBase;

	// Левый верхний угол, ширина, высота (обновляется при перерисовке).
	// Служебная инфа, поэтому mutable (символ сам от этого не меняется).
	mutable int x;
	mutable int y;
	mutable int width;
	mutable int height;
};