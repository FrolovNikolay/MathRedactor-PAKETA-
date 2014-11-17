// Автор: Фролов Николай

// Описание: класс-механизм, отвечающий за выделение частей текста.

#pragma once

#include <vector>
#include "LineOfSymbols.h"

class CItemSelector {
public:
	CItemSelector( std::vector<CLineOfSymbols>& _content );
	~CItemSelector();

	// Установка начальной позиции курсора
	void SetStartPosition( int x, int y, int moveX, int moveY );
	// Установка текущей позиции курсора
	void SetCurrentPosition( int x, int y, int moveX, int moveY );
	// Проверить наличие выделения
	bool HasSelection() const;
	// Выделить символы, в соответствии с заданными в классе координатами
	void MakeSelection( HDC displayHandle, int width, int height, int moveX, int moveY ) const;
	// Отменить выделение
	void ResetSelection();
	//
	bool IsGlobalSelection() const { return baseLine == 0; }
	//
	void GetGlobalSelectionInfo( int& startLine, int& startSymbol, int& lastLine, int& lastSymbol ) const;
	//
	void GetLocalSelectionInfo( CLineOfSymbols*& baseLine, int& startSymbol, int& lastSymbol );

private:
	// содержимое окна
	std::vector<CLineOfSymbols>& content;
	// координаты начала выделения
	int startX, startY;
	// координаты конца выделения
	int endX, endY;
	// если линия не одна из основных, то сюда попадет подлиния
	const CLineOfSymbols* baseLine;

	CItemSelector();

	void findBaseLine( int x, int y, int moveX, int moveY );

	void isLineBase( CLineOfSymbols* currentBaseLine, int x, int y );

	void getItemInfo( int x, int y, int& line, int& symbolIdx ) const;

	void drawSelection( HDC displayHandle, int width, int height, int leftTopX, int leftTopY, int firstLineHeight,
		int rightBotX, int RightBotY, int lastLineHeight, int moveX, int moveY ) const;

	void drawSelection( HDC displayHandle, int leftTopX, int leftTopY, int rightBotX, int rightBotY,
		int moveX, int moveY ) const;
};