// Автор: Николай Фролов.

#include "ItemSelector.h"

CItemSelector::CItemSelector( const CPositionFinder& _finder, const std::vector<CLineOfSymbols>& _content ) 
	: finder( _finder )
	, content( _content )
	, start( 0 )
	, end( 0 )
{ }

// Установка начальной позиции курсора.
void CItemSelector::SetStartPosition( int x, int y, int firstEnableSymbol )
{
	std::shared_ptr<CSymbolPosition> tmp( finder.FindPosition( x, y, firstEnableSymbol ) );
	// Если индекс -1, то пытаемся выделить в пустой подстроке символа, значит выделять нечего.
	if( tmp->Index != -1 ) {
		start.swap( tmp );
	}
}

// Установка текущей позиции курсора.
void CItemSelector::SetCurrentPosition( int x, int y, int firstEnableSymbol )
{
	// Если начальное положение не задано, то ничего не выделяем.
	if( start != 0 ) {
		std::shared_ptr<CSymbolPosition> tmp( finder.FindPosition( x, y, firstEnableSymbol, start.get() ) );
		end.swap( tmp );
	}
}

// Снять выделение.
void CItemSelector::ResetSelection()
{
	std::shared_ptr<CSymbolPosition> tmpStart( 0 );
	start.swap( tmpStart );
	std::shared_ptr<CSymbolPosition> tmpEnd( 0 );
	end.swap( tmpEnd );
}

// Информация о выделенных объектах.
CItemSelector::CSymbolInterval CItemSelector::GetSelectionInfo() const
{
	// В случае выделения по всему редактору нужна дополнительна обработка.
	if( start->GetParent() == 0 ) {
		int startIdx = -1;
		while( &content[++startIdx] != start->CurrentLine )	{ }

		int endIdx = -1;
		while ( &content[++endIdx] != end->CurrentLine ) { }

		CSymbolPosition tmpStart( *start );
		CSymbolPosition tmpEnd( *end );
		if( startIdx > endIdx || tmpStart.Index > tmpEnd.Index ) {
			std::swap( tmpStart, tmpEnd );
			std::swap( startIdx, endIdx );
		}
		if( tmpStart.CurrentLine == &content[content.size() - 1] && tmpStart.Index == tmpStart.CurrentLine->Length() ) {
			return std::make_pair( CSymbolPosition( -1, tmpStart ), CSymbolPosition( -1, tmpEnd ) );
		} else {
			goToNextSymbol( startIdx, tmpStart );
			goToPrevSymbol( endIdx, tmpEnd );
			if( startIdx > endIdx ) {
				return std::make_pair( CSymbolPosition( -1, tmpStart ), CSymbolPosition( -1, tmpEnd ) );
			} else {
				return std::make_pair( tmpStart, tmpEnd );
			}
		}
	} else {
		// Упорядочиваем позиции символов.
		if( start->Index > end->Index ) {
			return std::make_pair( *end, *start );
		} else {
			return std::make_pair( *start, *end );
		}
	}
}

// Переход к следующему непустому символу.
void CItemSelector::goToNextSymbol( int& lineIdx, CSymbolPosition& position ) const
{
	if( position.CurrentLine->Length() == position.Index ) {
		for( ++lineIdx; lineIdx < static_cast<int>( content.size() ); ++lineIdx ) {
			if( content[lineIdx].Length() != 0 ) {
				break;
			}
		}
		position = CSymbolPosition( 0, &content[lineIdx] );
	}
}

// Переход к предыдущему непустому символу.
void CItemSelector::goToPrevSymbol( int& lineIdx, CSymbolPosition& position ) const
{
	if( position.CurrentLine->Length() == position.Index ) {
		if( position.CurrentLine->Length() != 0 ) {
			--position.Index;
			return;
		}
		for( --lineIdx; lineIdx > 0; --lineIdx ) {
			if( content[lineIdx].Length() != 0 ) {
				break;
			}
		}
		position = CSymbolPosition( content[lineIdx].Length() - 1, &content[lineIdx] );
	}
}