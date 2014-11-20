// Автор: Николай Фролов

#include "ItemSelector.h"
#include "instruments.h"
#include <typeinfo>
#include "SimpleSymbol.h"
#include "FractionSymbol.h"
#include "IndexSymbol.h"
#include "SigmaSymbol.h"

CItemSelector::CItemSelector( std::vector<CLineOfSymbols>& _content ) :
	content( _content ),
	startX( -1 ),
	startY( -1 ),
	endX( -1 ),
	endY( -1 )
{
}

bool CItemSelector::HasSelection() const
{
	if( ( endX == -1 && endY == -1 ) ) {
		return false;
	} else {
		return true;
	}
}

void CItemSelector::ResetSelection()
{
	startX = -1;
	startY = -1;
	endX = -1;
	endY = -1;
}

void CItemSelector::SetStartPosition( int x, int y, int moveX, int moveY )
{
	startX = x + moveX;
	startY = y + moveY;
	findBaseLine( x, y, moveX, moveY );
}

void CItemSelector::SetCurrentPosition( int x, int y, int moveX, int moveY )
{
	endX = x + moveX;
	endY = y + moveY;
}

void CItemSelector::MakeSelection( HDC displayHandle, int width, int height, int moveX, int moveY ) const
{
	// делим на два случая: выделение внутри одной из основных строк, либо мы выделяем внутри сложного символа
	if( baseLine == 0 ) {
		// Определяем положение символов во внутренней структуре
		int startLine = 0;
		int firstMarkedSymbol = 0;
		int endLine = 0;
		int lastMarkedSymbol = 0;
		getItemInfo( startX, startY, startLine, firstMarkedSymbol );
		getItemInfo( endX, endY, endLine, lastMarkedSymbol );

		// определяем начало и конец в нашем выделении
		if( startLine > endLine ) {
			std::swap( startLine, endLine );
			std::swap( firstMarkedSymbol, lastMarkedSymbol );
		} else if( startLine == endLine && firstMarkedSymbol > lastMarkedSymbol ) {
			std::swap( startLine, endLine );
			std::swap( firstMarkedSymbol, lastMarkedSymbol );
		}

		// Под выделение ничего не попало
		if( startLine == content.size() ) {
			return;
		}

		// координаты левого верхнего угла первого символа
		int leftTopX = 0;
		int leftTopY = 0;
		int firstLineHeight = content[startLine].GetHeight();
		for( int i = 0; i < startLine; ++i ) {
			leftTopY += content[i].GetHeight();
		}
		for( int i = 0; i < firstMarkedSymbol; ++i ) {
			leftTopX += content[startLine][i]->GetWidth();
		}

		// если конец выделения ушел за конец текста, то выделяем по последний символ включительно
		if( endLine == content.size() ) {
			endLine = content.size() - 1;
			lastMarkedSymbol = content[endLine].Length() - 1;
		}

		// координаты правого нижнего угла последнего символа
		int rightBotX = 0;
		int rightBotY = 0;
		int lastLineHeight = content[endLine].GetHeight();
		for( int i = 0; i <= endLine; ++i) {
			rightBotY += content[i].GetHeight();
		}
		for( int i = 0; i <= lastMarkedSymbol; ++i ) {
			rightBotX += content[endLine][i]->GetWidth();
		}

		drawSelection( displayHandle, width, height, leftTopX, leftTopY, firstLineHeight, rightBotX,
			rightBotY, lastLineHeight, moveX, moveY );
	} else {
		// Определяем начало и конец выделения
		int startPos = max( startX + moveX , 0);
		int endPos = max( endX + moveX, 0 );
		if( startPos > endPos ) {
			std::swap( startPos, endPos );
		}

		// Находим первый символ выделения в подстроке
		int currentX = baseLine->GetX();
		int startSymbolIdx = 0;
		for( ; startSymbolIdx < baseLine->Length(); ++startSymbolIdx ) {
			currentX += (*baseLine)[startSymbolIdx]->GetWidth();
			if( currentX >= startPos ) {
				break;
			}
		}
		// Первый символ выделения за пределами строки - выделения нет
		if( startSymbolIdx == baseLine->Length() ) {
			return;
		}
		// Координаты левого верхнего угла первого символа
		int leftTopX = (*baseLine)[startSymbolIdx]->GetX();
		int leftTopY = baseLine->GetY();

		// Аналогично для второго символа
		currentX = baseLine->GetX();
		int lastSymbolIdx = 0;
		for( ; lastSymbolIdx < baseLine->Length(); ++lastSymbolIdx ) {
			currentX += (*baseLine)[lastSymbolIdx]->GetWidth();
			if( currentX >= endPos ) {
				break;
			}
		}
		// Если ушли за границу - то выделяем до конца
		if( lastSymbolIdx == baseLine->Length() ) {
			--lastSymbolIdx;
		}
		// Находим координаты правого нижнего угла для последнего символа
		int rightBotX = (*baseLine)[lastSymbolIdx]->GetX() + (*baseLine)[lastSymbolIdx]->GetWidth();
		int rightBotY = baseLine->GetY() + baseLine->GetHeight();

		drawSelection( displayHandle, leftTopX, leftTopY, rightBotX, rightBotY, moveX, moveY );
	}
}

void CItemSelector::GetGlobalSelectionInfo( int& startLine, int& startSymbol, int& lastLine, int& lastSymbol ) const
{
	getItemInfo( startX, startY, startLine, startSymbol );
	getItemInfo( endX, endY, lastLine, lastSymbol );
	if( startLine > lastLine ) {
		std::swap( startLine, lastLine );
		std::swap( startSymbol, lastSymbol );
	} else if( startLine == lastLine && startSymbol > lastSymbol ) {
		std::swap( startSymbol, lastSymbol );
	}
}

void CItemSelector::GetLocalSelectionInfo( CLineOfSymbols*& outBaseLine, int& startSymbol, int& lastSymbol )
{
	int startPos = startX;
	int endPos = endX;
	if( startPos > endPos ) {
		std::swap( startPos, endPos );
	}
	if( baseLine == 0 ) {
		baseLine = outBaseLine;
	}
	outBaseLine = const_cast<CLineOfSymbols*>( baseLine );
	if( baseLine->Length() == 0 ) {
		startSymbol = 0;
		lastSymbol = -1;
		return;
	}

	// Находим первый символ выделения в подстроке
	int currentX = baseLine->GetX();
	startSymbol = 0;
	for( ; startSymbol < baseLine->Length(); ++startSymbol ) {
		currentX += (*baseLine)[startSymbol]->GetWidth();
		if( currentX >= startPos ) {
			break;
		}
	}

	currentX = baseLine->GetX();
	lastSymbol = 0;
	for( ; lastSymbol < baseLine->Length(); ++lastSymbol ) {
		currentX += (*baseLine)[lastSymbol]->GetWidth();
		if( currentX >= endPos ) {
			break;
		}
	}
	baseLine = 0;
}

// Определяем базовую подлинию начала выделения во внутренней структуре
void CItemSelector::findBaseLine( int x, int y, int moveX, int moveY )
{
	int lineIdx = 0;
	int symbolIdx = 0;
	getItemInfo( x + moveX, y + moveY, lineIdx, symbolIdx );
	// Если выделение начинается за границей существующих строк / символов в строке, то выделяем по всему документу
	if( lineIdx == content.size() || symbolIdx == -1 ) {
		baseLine = 0;
		return;
	} else {
		isLineBase( &content[lineIdx], x, y );
	}
	// если в итоге во внутренней структуре мы не продвинулись дальше "обычных" строк, то выделяем по всему документу
	if( baseLine == &content[lineIdx] ) {
		baseLine = 0;
	}
}

// рекурсивно проверяем является ли очередная линия итоговой для выделения
void CItemSelector::isLineBase( CLineOfSymbols* currentBaseLine, int x, int y )
{
	// находим символ, в котором может оказаться очередная подстрока
	int currentX = currentBaseLine->GetX();
	int symbolIdx = 0;
	for( ; symbolIdx < currentBaseLine->Length(); ++symbolIdx ) {
		currentX += (*currentBaseLine)[symbolIdx]->GetWidth();
		if( currentX >= x ) {
			break;
		}
	}
	std::vector<CLineOfSymbols*> substings;
	// если символ простой или мы ушли за границу строки, то текущая линия уже является стартовой для выделения
	if( currentX < x || currentBaseLine ->Length() == 0) {
		baseLine = currentBaseLine;
		return;
	// иначе в зависимости от типа символа смотрим, не нужно ли перейти на одну из "более внутренних" строк
	} else {
		(*currentBaseLine)[symbolIdx]->GetSubstrings( substings );
		if( substings.size() == 0 ) {
			baseLine = currentBaseLine;
			return;
		} else {
			for( int i = 0; i < static_cast<int>( substings.size() ); ++i ) {
				if( isLineContainPoint(substings[i], x, y) ) {
					isLineBase( substings[i], x, y );
					return;
				}
			}
			baseLine = currentBaseLine;
			return;
		}
	}
}

// находим положение символа, соответствующего данным координатам, во внутренней структуре
void CItemSelector::getItemInfo( int x, int y, int& lineIdx, int& symbolIdx ) const
{
	int currentY = 0;
	for( lineIdx = 0; lineIdx < static_cast<int>( content.size() ); ++lineIdx ) {
		currentY += content[lineIdx].GetHeight();
		if( currentY >= y ) {
			break;
		}
	}
	if( currentY < y ) {
		return;
	}
	int currentX = 0;
	for( symbolIdx = 0; symbolIdx < content[lineIdx].Length(); ++symbolIdx ) {
		currentX += content[lineIdx][symbolIdx]->GetWidth();
		if( currentX >= x ) {
			break;
		} 
	}
	if( currentX < x ) {
		++lineIdx ;
		symbolIdx = -1;
	}
}

// выделение во внутренних подстроках по координам первого и посленего символа
void CItemSelector::drawSelection( HDC displayHandle, int leftTopX, int leftTopY, int rightBotX, int rightBotY,
	int moveX, int moveY ) const
{
	HBRUSH selectionBrush = ::CreateSolidBrush( RGB( 100, 100, 255 ) );
	HBRUSH lastBrush = static_cast<HBRUSH>( ::SelectObject( displayHandle, selectionBrush ) );
	HPEN newPen = CreatePen( PS_SOLID, 1, RGB( 100, 100, 255 ) );
	HPEN oldPen = static_cast<HPEN>( ::SelectObject( displayHandle, newPen ) );

	::Rectangle( displayHandle, leftTopX, leftTopY, rightBotX, rightBotY );

	::SelectObject( displayHandle, lastBrush );
	::SelectObject( displayHandle, oldPen );
	::DeleteObject( newPen );
	::DeleteObject( selectionBrush );
}

// прорисовка выделения для имеющихся в классе start/current позиций
void CItemSelector::drawSelection( HDC displayHandle, int width, int height, int leftTopX, int leftTopY,
		int firstLineHeight, int rightBotX, int rightBotY, int lastLineHeight, int moveX, int moveY ) const
{
	HBRUSH selectionBrush = ::CreateSolidBrush( RGB( 100, 100, 255 ) );
	HBRUSH lastBrush = static_cast<HBRUSH>( ::SelectObject( displayHandle, selectionBrush ) );
	HPEN newPen = CreatePen( PS_SOLID, 1, RGB( 100, 100, 255 ) );
	HPEN oldPen = static_cast<HPEN>( ::SelectObject( displayHandle, newPen ) );

	leftTopX = max( 0, leftTopX + moveX );
	leftTopY = leftTopY + moveY;
	if( leftTopY <= 0 ) {
		firstLineHeight = max( 0, firstLineHeight + leftTopY );
		leftTopY = 0;
	}
	rightBotX = max( 0, rightBotX + moveX );
	rightBotY = max( 0, rightBotY + moveY );

	// вся прорисовка включает в себя три прямоугольника, зависящих от координат первого и последнего символа в выделении
	if( leftTopX <= rightBotX ) {
		::Rectangle( displayHandle, leftTopX, leftTopY, rightBotX, rightBotY );
		::Rectangle( displayHandle, rightBotX, leftTopY, width, rightBotY - lastLineHeight );
		::Rectangle( displayHandle, 0, leftTopY + firstLineHeight, leftTopX, rightBotY );
	} else {
		::Rectangle( displayHandle, leftTopX, leftTopY, width, rightBotY - lastLineHeight );
		::Rectangle( displayHandle, rightBotX, leftTopY + firstLineHeight, leftTopX, rightBotY - lastLineHeight );
		::Rectangle( displayHandle, 0, leftTopY + firstLineHeight, rightBotX, rightBotY );
	}

	::SelectObject( displayHandle, lastBrush );
	::SelectObject( displayHandle, oldPen );
	::DeleteObject( newPen );
	::DeleteObject( selectionBrush );
}