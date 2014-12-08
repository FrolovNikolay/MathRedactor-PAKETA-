#include "LineOfSymbols.h"
#include "SimpleSymbol.h"
#include <assert.h>
#include <typeinfo>

CLineOfSymbols::CLineOfSymbols( int _simpleSymbolHeight, bool _isBase ) :
	height( _simpleSymbolHeight ),
	baselineOffset( 0 ),
	simpleSymbolHeight( _simpleSymbolHeight ),
	parent( 0 ),
	isBase( isBase )
{ }

CLineOfSymbols::CLineOfSymbols( const CLineOfSymbols& src ) :
	height( src.height ),
	baselineOffset( src.baselineOffset ),
	simpleSymbolHeight( src.simpleSymbolHeight ),
	parent( 0 )
{
	for( int i = 0; i < src.Length(); ++i ) {
		assert( src[i] != 0 );
		PushBack( src[i]->Clone( this ) );
	}
}

CLineOfSymbols& CLineOfSymbols::operator=( const CLineOfSymbols& src )
{
	if( &src == this ) {
		return *this;
	}

	CLineOfSymbols tmp( src );
	std::swap( tmp.arrayOfSymbolPtrs, arrayOfSymbolPtrs );
	std::swap( tmp.baselineOffset, baselineOffset );
	std::swap( tmp.height, height );
	std::swap( tmp.width, width );
	std::swap( tmp.parent, parent );
	std::swap( tmp.x, x );
	std::swap( tmp.y, y );
	return *this;
}

CLineOfSymbols::~CLineOfSymbols( )
{
	for( int i = 0; i < static_cast<int>( arrayOfSymbolPtrs.size() ); ++i ) {
		delete arrayOfSymbolPtrs[i];
	}
}

void CLineOfSymbols::Insert( int index, CSymbol* symbol )
{
	arrayOfSymbolPtrs.insert( arrayOfSymbolPtrs.begin() + index, symbol );
	Recalculate();
}

void CLineOfSymbols::Concatenate( CLineOfSymbols* line ) 
{
    std::vector<CSymbol*>::iterator it;
    for( it = line->arrayOfSymbolPtrs.begin(); it != line->arrayOfSymbolPtrs.end(); ++it ) {
        arrayOfSymbolPtrs.push_back( *it );
    }
}

void CLineOfSymbols::Delete( int index )
{
	delete( arrayOfSymbolPtrs[index] );
	arrayOfSymbolPtrs.erase( arrayOfSymbolPtrs.begin() + index );
	Recalculate();
}

void CLineOfSymbols::Draw( HDC displayHandle, int posX, int posY ) const
{
	if( !isBase && arrayOfSymbolPtrs.empty() ) {
		x = posX;
		y = posY;
		height = simpleSymbolHeight;
		width = simpleSymbolHeight;
		HBRUSH voidBrush = ::CreateSolidBrush( RGB( 0xF0, 0xF0, 0xF0 ) );
		HBRUSH oldBrush = static_cast<HBRUSH>( ::SelectObject( displayHandle, voidBrush ) );
		HPEN voidPen = CreatePen( PS_SOLID, 1, RGB( 0xF0, 0xF0, 0xF0 ) );
		HPEN oldPen = static_cast<HPEN>( ::SelectObject( displayHandle, voidPen ) );
		::Rectangle( displayHandle, posX, posY, posX + width, posY + height );
		::SelectObject( displayHandle, oldBrush );
		::SelectObject( displayHandle, oldPen );
		::DeleteObject( voidBrush );
		::DeleteObject( voidPen );
	} else {
		//Устанавливаем шрифт (получаем текущий и обновляем высоту символа)
		HFONT oldFont = (HFONT)::GetCurrentObject( displayHandle, OBJ_FONT );
		assert( oldFont != 0 );
	
		x = posX;
		y = posY;

		LOGFONT fontInfo;
		::GetObject( oldFont, sizeof(LOGFONT), &fontInfo );
		fontInfo.lfHeight = simpleSymbolHeight;
		HFONT font = ::CreateFontIndirect( &fontInfo );
		assert( font != 0 );
		oldFont = (HFONT)::SelectObject( displayHandle, font );

		// Отрисовка
		for( int i = 0; i < static_cast<int>( arrayOfSymbolPtrs.size() ); ++i ) {
			assert( arrayOfSymbolPtrs[i] != 0 );
			arrayOfSymbolPtrs[i]->Draw( displayHandle, posX, posY + baselineOffset, simpleSymbolHeight );
			posX += arrayOfSymbolPtrs[i]->CalculateWidth( displayHandle );
		}
	
		width = posX - x;

		if( width == 0 ) {
			TEXTMETRIC textMetric;
			::GetTextMetrics( displayHandle, &textMetric );
			width = textMetric.tmAveCharWidth;
		}

		//Возвращаем старый шрифт, удаляем созданный
		::SelectObject( displayHandle, oldFont );
		::DeleteObject( font );
	}
}

int CLineOfSymbols::CalculateWidth( HDC displayHandle ) const
{
	if( !isBase && arrayOfSymbolPtrs.empty() ) {
		width = simpleSymbolHeight;
		return width;
	}
	//Устанавливаем шрифт (получаем текущий и обновляем высоту символа)
	HFONT oldFont = (HFONT)::GetCurrentObject( displayHandle, OBJ_FONT );
	assert( oldFont != 0 );

	LOGFONT fontInfo;
	::GetObject( oldFont, sizeof(LOGFONT), &fontInfo );
	fontInfo.lfHeight = simpleSymbolHeight;
	HFONT font = ::CreateFontIndirect( &fontInfo );
	assert( font != 0 );
	oldFont = (HFONT)::SelectObject( displayHandle, font );

	int result = 0;
	for( int i = 0; i < static_cast<int>( arrayOfSymbolPtrs.size() ); ++i ) {
		assert( arrayOfSymbolPtrs[i] != 0 );
		result += arrayOfSymbolPtrs[i]->CalculateWidth( displayHandle );
	}

	if( result == 0 ) {
		TEXTMETRIC textMetric;
		::GetTextMetrics( displayHandle, &textMetric );
		result = textMetric.tmAveCharWidth;
	}

	//Возвращаем старый шрифт, удаляем созданный
	::SelectObject( displayHandle, oldFont );
	::DeleteObject( font );

	return result;
}

void CLineOfSymbols::Recalculate()
{
	if( !isBase && arrayOfSymbolPtrs.empty() ) {
		height = simpleSymbolHeight;
		int descent = simpleSymbolHeight;
		baselineOffset = 0;
	} else {
		int descent = simpleSymbolHeight;
		baselineOffset = 0;
		for( int i = 0; i < Length(); ++i ) {
			descent = max( descent, arrayOfSymbolPtrs[i]->GetDescent( simpleSymbolHeight ) );
			baselineOffset = max( baselineOffset, arrayOfSymbolPtrs[i]->GetBaselineOffset( simpleSymbolHeight ) );
		}

		height = descent + baselineOffset;
	}
	
	if( parent != 0 ) {
		parent->Recalculate();
	}
}

// Преобразование строки в необходимый плоттеру формат.
std::string CLineOfSymbols::ToPlotterString( int firstEnablePosition ) const
{
	std::string result = "";
	for( int i = firstEnablePosition; i < static_cast<int>( arrayOfSymbolPtrs.size() ); ++i ) {
		result += arrayOfSymbolPtrs[i]->ToPlotterString();
	}
	return result;
}

// Преобразование строки в Latex формат.
std::string CLineOfSymbols::ToLatexString( int firstEnableSymbol ) const
{
	std::string result = "";
	for( int i = firstEnableSymbol; i < static_cast<int>( arrayOfSymbolPtrs.size() ); ++i ) {
		result += arrayOfSymbolPtrs[i]->ToLatexString();
	}
	return result;
}