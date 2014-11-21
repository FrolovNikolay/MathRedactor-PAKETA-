// Автор: Фролов Николай.

#include "instruments.h"

bool IsLineContainPoint( const CLineOfSymbols* line, int x, int y )
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