#include "LineOfSymbols.h"
#include "SimpleSymbol.h"
#include <assert.h>
#include <typeinfo>

CLineOfSymbols::CLineOfSymbols( int _simpleSymbolHeight ) :
	height( _simpleSymbolHeight ),
	baselineOffset( 0 ),
	simpleSymbolHeight( _simpleSymbolHeight ),
	parent( 0 )
{}

CLineOfSymbols::CLineOfSymbols( const CLineOfSymbols& src ) :
	height( src.height ),
	baselineOffset( src.baselineOffset ),
	simpleSymbolHeight( src.simpleSymbolHeight ),
	parent( 0 )
{

	for( int i = 0; i < src.Length(); ++i ) {
		assert( src[i] != 0 );
		Push( src[i]->Clone( this ), i );
	}
}

CLineOfSymbols& CLineOfSymbols::operator=( const CLineOfSymbols& src )
{
	if( &src == this ) {
		return *this;
	}

	height = src.height;
	baselineOffset = src.baselineOffset;
	simpleSymbolHeight = src.simpleSymbolHeight;
	for( int i = 0; i < src.Length(); ++i ) {
		assert( src[i] != 0 );
		Push( src[i]->Clone( this ), i );
	}
	return *this;
}

CLineOfSymbols::~CLineOfSymbols( )
{
	for( int i = 0; i < static_cast<int>( arrayOfSymbolPtrs.size() ); ++i ) {
		delete arrayOfSymbolPtrs[i];
	}
}

void CLineOfSymbols::Push( CSymbol* symbol, int index )
{
	if( index < 0 ) {
		//TODO: add an error or exception
		return;
	}
	if( index >= static_cast<int>( arrayOfSymbolPtrs.size() ) ) {
		arrayOfSymbolPtrs.push_back( symbol );
	} else {
		arrayOfSymbolPtrs.insert( arrayOfSymbolPtrs.begin() + index, symbol );
	}
	Recalculate();
}

void CLineOfSymbols::Pop( int index )
{
	delete( arrayOfSymbolPtrs[index] );
	arrayOfSymbolPtrs.erase( arrayOfSymbolPtrs.begin() + index );
	Recalculate();
}

void CLineOfSymbols::Draw( HDC displayHandle, int posX, int posY ) const
{

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

	//Отрисовка
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

int CLineOfSymbols::CalculateWidth( HDC displayHandle ) const
{
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
	int descent = simpleSymbolHeight;
	baselineOffset = 0;
	for( int i = 0; i < Length(); ++i ) {
		descent = max( descent, arrayOfSymbolPtrs[i]->GetDescent( simpleSymbolHeight ) );
		baselineOffset = max( baselineOffset, arrayOfSymbolPtrs[i]->GetBaselineOffset( simpleSymbolHeight ) );
	}

	height = descent + baselineOffset;
	
	if( parent != 0 ) {
		parent->Recalculate();
	}
}