// Автор: Николай Фролов.

#include "PositionFinder.h"

// Получить позицию, соответствующую указанной точке.
CSymbolPosition* CPositionFinder::FindPosition( int x, int y, int firstEnableSymbol, CSymbolPosition* baseLine ) const
{
	// Если задана родительская линия, и она не базовая, то просто ищем в ней.
	if( baseLine != 0 && baseLine->GetParent() != 0 ) {
		return positionInLine( x, y, baseLine );
	} else {
		int lineIdx = 0;
		int currentY = 0;
		if( baseLine == 0 ) {
			for( lineIdx = 0; lineIdx < static_cast<int>( content.size() ); ++lineIdx ) {
				currentY += content[lineIdx].GetHeight();
				if( currentY >= y ) {
					break;
				}
			}
			if( currentY < y ) {
				return new CSymbolPosition( content[content.size() - 1].Length(), &content[content.size() - 1] );
			}
		} else {
			lineIdx = -1;
			while( &content[++lineIdx] != baseLine->CurrentLine ) { }
		}
		int symbolIdx = firstEnableSymbol;
		int currentX = content[lineIdx][symbolIdx - 1]->GetX() - content[0].GetX() + content[lineIdx][symbolIdx - 1]->GetWidth();
		for( ; symbolIdx < content[lineIdx].Length(); ++symbolIdx ) {
			currentX += content[lineIdx][symbolIdx]->GetWidth();
			if( currentX >= x ) {
				break;
			} 
		}
		if( symbolIdx == content[lineIdx].Length() ) {
			return new CSymbolPosition( content[lineIdx].Length(), &content[lineIdx] );
		}
		if( baseLine != 0 ) {
			return new CSymbolPosition( symbolIdx, &content[lineIdx] );
		} else {
			return exactPosition( x, y, new CSymbolPosition( symbolIdx, &content[lineIdx] ) );
		}
	}
}

// Попытаться уточнить связанную с координатами позицию.
CSymbolPosition* CPositionFinder::exactPosition( int x, int y, CSymbolPosition* parent ) const
{
	std::vector<const CLineOfSymbols*> substings;
	if( parent->Index == -1 ) {
		return parent;
	}
	( *parent->CurrentLine )[parent->Index]->GetSubstrings( substings );
	if( substings.size() == 0 ) {
		return parent;
	} else {
		// Смещение связанное со скроллом.
		int moveX = content[0].GetX();
		int moveY = content[0].GetY();
		for( size_t i = 0; i < substings.size(); ++i ) {
			if( IsLineContainPoint( substings[i], x + moveX, y + moveY ) ) {
				std::unique_ptr<CSymbolPosition> tmp( new CSymbolPosition( 0, substings[i], parent ) );
				return exactPosition( x, y, positionInLine( x, y, tmp.get() ) );
			}
		}
		return parent;
	}
}

// Найти значение позиции символа внутри строки.
CSymbolPosition* CPositionFinder::positionInLine( int x, int y, CSymbolPosition* baseLine ) const
{ 
	if( baseLine->CurrentLine->Length() == 0 ) {
		return new CSymbolPosition( -1, *baseLine );
	}
	int currentX = baseLine->CurrentLine->GetX() - content[0].GetX();
	int symbolIdx;
	for( symbolIdx = 0; symbolIdx < baseLine->CurrentLine->Length(); ++symbolIdx ) {
		currentX += ( *baseLine->CurrentLine )[symbolIdx]->GetWidth();
		if( currentX > x ) {
			return new CSymbolPosition( symbolIdx, *baseLine );
		}
	}
	return new CSymbolPosition( baseLine->CurrentLine->Length() - 1, *baseLine ); 
}

// Проверям попадает ли данная точка в данную линию.
bool CPositionFinder::IsLineContainPoint( const CLineOfSymbols* line, int x, int y )
{
	int leftTopX = line->GetX();
	int leftTopY = line->GetY();
	int height = line->GetHeight();
	int width = line->GetWidth();
	if( leftTopX <= x && x <= leftTopX + width && leftTopY <= y && y <= leftTopY + height ) {
		return true;
	} else {
		return false;
	}
}