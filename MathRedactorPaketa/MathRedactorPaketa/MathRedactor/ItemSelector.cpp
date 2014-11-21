// Автор: Николай Фролов.

#include "ItemSelector.h"
#include "PositionFinder.h"

CItemSelector::CItemSelector( const CPositionFinder& _finder ) 
	: finder( _finder )
	, start( 0 )
	, end( 0 )
{ }

void CItemSelector::SetStartPosition( int x, int y )
{
	start = finder.FindPosition( x, y );
}

void CItemSelector::SetCurrentPosition( int x, int y )
{
	if( end != 0 ) {
		delete end;
	}
	end = finder.FindPosition( x, y, start );
}