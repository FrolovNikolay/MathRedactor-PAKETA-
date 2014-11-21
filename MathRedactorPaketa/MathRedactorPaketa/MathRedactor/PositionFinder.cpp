// Автор: Николай Фролов.

#include "PositionFinder.h"
#include "instruments.h"

const CSymbolPosition* CPositionFinder::FindPosition( int x, int y, const CSymbolPosition* baseLine ) const
{
	if( baseLine != 0 && baseLine->GetParent() != 0 ) {
		return positionInLine( x, y, baseLine );
	} else {
		int lineIdx = 0;
		int symbolIdx = 0;
		int currentY = 0;
		for( lineIdx = 0; lineIdx < static_cast<int>( content.size() ); ++lineIdx ) {
			currentY += content[lineIdx].GetHeight();
			if( currentY >= y ) {
				break;
			}
		}
		if( currentY < y ) {
			return new CSymbolPosition( content[content.size() - 1].Length() - 1, content[content.size() - 1] );
		}
		int currentX = 0;
		for( symbolIdx = 0; symbolIdx < content[lineIdx].Length(); ++symbolIdx ) {
			currentX += content[lineIdx][symbolIdx]->GetWidth();
			if( currentX >= x ) {
				break;
			} 
		}
		if( currentX < x ) {
			return new CSymbolPosition( content[lineIdx].Length() - 1, content[lineIdx] );
		}
		if( baseLine != 0 ) {
			return new CSymbolPosition( symbolIdx, content[lineIdx] );
		} else {
			return exactPosition( x, y, new CSymbolPosition( symbolIdx, content[lineIdx] ) );
		}
	}
}

const CSymbolPosition* CPositionFinder::exactPosition( int x, int y, const CSymbolPosition* parent ) const
{
	std::vector<const CLineOfSymbols*> substings;
	parent->CurrentLine[parent->Index]->GetSubstrings( substings );
	if( substings.size() == 0 ) {
		return parent;
	} else {
		const CSymbolPosition* baseParent = parent;
		while( baseParent->GetParent() != 0 ) {
			baseParent = baseParent->GetParent();
		}
		int moveX = -baseParent->CurrentLine.GetX();
		int moveY = -baseParent->CurrentLine.GetY();
		for( int i = 0; i < static_cast<int>( substings.size() ); ++i ) {
			if( IsLineContainPoint( substings[i], x - moveX, y - moveY ) ) {
				std::unique_ptr<const CSymbolPosition> tmp( new CSymbolPosition( 0, *substings[i], parent ) );
				return positionInLine( x, y, tmp.get() );
			}
		}
		return parent;
	}
}

const CSymbolPosition* CPositionFinder::positionInLine( int x, int y, const CSymbolPosition* baseLine ) const
{
	if( baseLine->CurrentLine.Length() == 0 ) {
		return new CSymbolPosition( -1, *baseLine );
	}
	int currentX = baseLine->CurrentLine.GetX();
	int symbolIdx;
	for( symbolIdx = 0; symbolIdx < baseLine->CurrentLine.Length(); ++symbolIdx ) {
		currentX += baseLine->CurrentLine[symbolIdx]->GetWidth();
		if( currentX > x ) {
			return new CSymbolPosition( symbolIdx, *baseLine );
		}
	}
	return new CSymbolPosition( baseLine->CurrentLine.Length() - 1, *baseLine ); 
}