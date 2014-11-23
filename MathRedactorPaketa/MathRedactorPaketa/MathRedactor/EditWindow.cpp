#include "EditWindow.h"
#include "SimpleSymbol.h"
#include "FractionSymbol.h"
#include "SigmaSymbol.h"
#include "IndexSymbol.h"
#include <typeinfo>
#include "instruments.h"
#include <assert.h>
#include <windowsx.h>
#include "SymbolPosition.h"

// класс CEditWindow
// константы

const wchar_t* CEditWindow::className = L"MathRedactorEditWindowClass";

// public методы

CEditWindow::CEditWindow() 
		: horizontalScrollUnit( 30 )
		, verticalScrollUnit( 15 )
		, simpleSymbolHeight( 50 )
		, content( 1, CLineOfSymbols( simpleSymbolHeight ) )
		, caret( this, &content[0] )
		, finder( content )
		, symbolSelector( finder, content )
		, drawer( content, symbolSelector, horizontalScrollUnit, verticalScrollUnit, simpleSymbolHeight )
{
	windowHandle = 0;
}

bool CEditWindow::RegisterClass( HINSTANCE classOwnerInstance )
{
	WNDCLASSEX classInfo;
	::ZeroMemory( &classInfo, sizeof( WNDCLASSEX ) );
	classInfo.cbSize = sizeof( WNDCLASSEX );
	classInfo.hInstance = classOwnerInstance;
	classInfo.lpszClassName = className;
	classInfo.style = CS_HREDRAW | CS_VREDRAW;
	classInfo.lpfnWndProc = &CEditWindow::windowProcedure;

	return ( ::RegisterClassEx( &classInfo ) != 0 );
}

HWND CEditWindow::Create( HWND parent, HINSTANCE ownerInstance )
{
	DWORD style = WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	windowHandle = ::CreateWindowEx( 0, className, 0, style, 0, 0, 0, 0, parent, 0, ownerInstance, this );

	return windowHandle;
}

void CEditWindow::Show( int nCmdShow )
{
	::ShowWindow( windowHandle, nCmdShow );
}

void CEditWindow::AddSymbol( CSymbol* symbol )
{
	if( symbolSelector.HasSelection() ) {
		removeSelectedItems();
		symbolSelector.ResetSelection();
		ShowCaret();
	}

	std::vector<CLineOfSymbols*> substrings;
	symbol->GetSubstrings( substrings );

	CLineOfSymbols* currentLine = GetCaretLine();

	for( int i = 0; i < static_cast<int>( substrings.size() ); ++i ) {
		substrings[i]->SetParent( currentLine );
	}
	currentLine->Insert( caret.GetIndex(), symbol );
	caret.Move( CD_Right );
	recalculateHorzScrollParams();
	recalculateVertScrollParams();
	::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE | RDW_ERASE );
}

void CEditWindow::AddSign( wchar_t sign )
{
	if( isSymbolAllowed( sign ) ) {
		AddSymbol( new CSimpleSymbol( sign ) );
	}
	ShowCaret();
	::InvalidateRect( windowHandle, 0, true );
}

void CEditWindow::RemoveSign()
{
	if( symbolSelector.HasSelection() ) {
		removeSelectedItems();
		symbolSelector.ResetSelection();
		ShowCaret();
	} else {
		CLineOfSymbols* currentLine = GetCaretLine();
		if( caret.GetIndex() > 0 ) {
			currentLine->Delete( caret.GetIndex() - 1 );
			caret.Move( CD_Left );
		} else if( getBaseLineIndex( currentLine ) > 0 ) {
			int lineIndex = getBaseLineIndex( currentLine );
			int pos = content[lineIndex - 1].Length();
			for( int i = 0; i < content[lineIndex].Length(); ++i ) {
				content[lineIndex - 1].PushBack( content[lineIndex][i]->Clone( &content[lineIndex - 1] ) );
			}
			content.erase( content.begin() + lineIndex );
			caret.MoveTo( CSymbolPosition( pos, &content[lineIndex - 1] ) );
			recalculateVertScrollParams();
		}
		currentLine->Recalculate();
		recalculateHorzScrollParams();
		::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
	}
}

void CEditWindow::NewLine()
{
	if( symbolSelector.HasSelection() ) {
		removeSelectedItems();
		symbolSelector.ResetSelection();
		ShowCaret();
	}

	CLineOfSymbols* currentLine = GetCaretLine();

	int lineIndex = getBaseLineIndex( currentLine );
	if( lineIndex != -1 ) {
		if( lineIndex == content.size() - 1 ) {
			content.push_back( CLineOfSymbols( simpleSymbolHeight, true ) );
		} else {
			content.insert( content.begin() + lineIndex + 1, CLineOfSymbols( simpleSymbolHeight, true ) );
		}
		for( int i = caret.GetIndex(); i < content[lineIndex].Length(); ++i ) {
			content[lineIndex + 1].Insert( i - caret.GetIndex(), content[lineIndex][i]->Clone( &content[lineIndex + 1] ) );
		}
		for( int i = content[lineIndex].Length() - 1; i >= caret.GetIndex(); --i ) {
			content[lineIndex].Delete( i );
		}
		caret.MoveTo( CSymbolPosition( 0, &content[lineIndex + 1] ) );
		recalculateVertScrollParams();
		::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
	}
}

void CEditWindow::ShowCaret()
{
	if( !caret.IsShown() ) {
		caret.Create();
		caret.Show();
	}
}

void CEditWindow::HideCaret()
{
	if( caret.IsShown() ) {
		caret.Hide();
		caret.Destroy();
	}
}

void CEditWindow::MoveCaret( CEditWindow::TCaretDirection direction )
{
	if( symbolSelector.HasSelection() ) {
		if( direction == CD_Left || direction == CD_Up ) {
			caret.MoveTo( symbolSelector.GetSelectionInfo().first );
		} else {
			caret.MoveTo( symbolSelector.GetSelectionInfo().second );
			caret.Move( CD_Right );
		}
		symbolSelector.ResetSelection();
		ShowCaret();
	} else {
		caret.Move( direction );
	}
	ShowCaret();
	::InvalidateRect( windowHandle, 0, true );
}

// Получить строку для плоттера.
std::string CEditWindow::CalculateStringForPlotter() const
{
	std::string result = "";
	for( int i = 0; i < static_cast<int>( content.size() ); ++i ) {
		result += content[i].ToPlotterString() + "\n";
	}
	return result;
}

// protected методы

void CEditWindow::OnWmDestroy() {
	::PostQuitMessage( 0 );
}

void CEditWindow::OnWmPaint()
{
	// мы запоминаем состояние каретки до перерисовки
	bool caretShown = caret.IsShown();
	if( caretShown ) {
		caret.Hide();
	}
	drawer.RePaint( windowHandle );
	// если каретка показывалась до перерисовки, то отображаем ее
	if( caretShown ) {
		caret.Show();
	}
}

void CEditWindow::OnWmHScroll( WPARAM wParam, LPARAM lParam )
{
	if( lParam == 0 ) {
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof( SCROLLINFO );
		scrollInfo.fMask = SIF_ALL;
		// меняем позицию скролла
		::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
		switch( LOWORD( wParam ) ) {
		case SB_LINELEFT:
			scrollInfo.nPos = max( scrollInfo.nPos - 1, scrollInfo.nMin );
			break;
		case SB_LINERIGHT:
			scrollInfo.nPos = min( scrollInfo.nPos + 1, scrollInfo.nMax );
			break;
		case SB_PAGELEFT:
			scrollInfo.nPos = max( scrollInfo.nPos - static_cast<int>( scrollInfo.nPage ), scrollInfo.nMin );
			break;
		case SB_PAGERIGHT:
			scrollInfo.nPos = min( scrollInfo.nPos + static_cast<int>( scrollInfo.nPage ), scrollInfo.nMax );
			break;
		case SB_THUMBTRACK:
			scrollInfo.nPos = scrollInfo.nTrackPos;
			break;
		}
		// запоминаем и устанавливаем новую позицию скролла
		int scrollPosition = scrollInfo.nPos;
		scrollInfo.fMask = SIF_POS;
		::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
		::SetScrollInfo( windowHandle, SB_HORZ, &scrollInfo, TRUE );
		// и перестраховка, на случай того, что Window сдвинет что-то не так, как ожидалось
		::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
		if( scrollInfo.nPos != scrollPosition ) {
			::ScrollWindow( windowHandle, horizontalScrollUnit * ( scrollPosition - scrollInfo.nPos ), 0, 0, 0 );
		}
	}
}

void CEditWindow::OnWmVScroll( WPARAM wParam, LPARAM lParam )
{
	if( lParam == 0 ) {
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof( SCROLLINFO );
		scrollInfo.fMask = SIF_ALL;
		// меняем позицию скролла
		::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
		switch( LOWORD( wParam ) ) {
		case SB_LINEUP:
			scrollInfo.nPos = max( scrollInfo.nPos - 1, scrollInfo.nMin );
			break;
		case SB_LINEDOWN:
			scrollInfo.nPos = min( scrollInfo.nPos + 1, scrollInfo.nMax );
			break;
		case SB_PAGEUP:
			scrollInfo.nPos = max( scrollInfo.nPos - static_cast<int>( scrollInfo.nPage ), scrollInfo.nMin );
			break;
		case SB_PAGEDOWN:
			scrollInfo.nPos = min( scrollInfo.nPos + static_cast<int>( scrollInfo.nPage ), scrollInfo.nMax );
			break;
		case SB_THUMBTRACK:
			scrollInfo.nPos = scrollInfo.nTrackPos;
			break;
		}
		// запоминаем и устанавливаем новую позицию скролла
		int scrollPosition = scrollInfo.nPos;
		scrollInfo.fMask = SIF_POS;
		::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
		::SetScrollInfo( windowHandle, SB_VERT, &scrollInfo, TRUE );

		// и перестраховка, на случай того, что Window сдвинет что-то не так, как ожидалось
		::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
		if( scrollInfo.nPos != scrollPosition ) {
			::ScrollWindow( windowHandle, 0, verticalScrollUnit * ( scrollPosition - scrollInfo.nPos ), 0, 0 );
		}
	}
}

void CEditWindow::OnWmSize( LPARAM lParam ) 
{
	recalculateHorzScrollParams();
	recalculateVertScrollParams();
}

void CEditWindow::OnLButDown( LPARAM lParam )
{
	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
	int moveX = scrollInfo.nPos * horizontalScrollUnit;
	::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
	int moveY = scrollInfo.nPos * verticalScrollUnit;

	int x = GET_X_LPARAM( lParam );
	int y = GET_Y_LPARAM( lParam );

	caret.MoveTo( x + moveX, y + moveY );
	symbolSelector.ResetSelection();
	symbolSelector.SetStartPosition( x + moveX, y + moveY );
	HideCaret();
}

void CEditWindow::OnLButUp( LPARAM lParam )
{
	int x = GET_X_LPARAM( lParam );
	int y = GET_Y_LPARAM( lParam );

	if( symbolSelector.HasSelection() ) {
		SCROLLINFO scrollInfo;
		::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
		scrollInfo.cbSize = sizeof( SCROLLINFO );
		scrollInfo.fMask = SIF_ALL;

		::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
		int moveX = scrollInfo.nPos * horizontalScrollUnit;
		::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
		int moveY = scrollInfo.nPos * verticalScrollUnit;

		symbolSelector.SetCurrentPosition( x + moveX, y + moveY );
	} else {
		ShowCaret();
	}
	InvalidateRect( windowHandle, 0, true );
}

void CEditWindow::OnLockedMouseMove( LPARAM lParam )
{
	if( symbolSelector.HasSelection() ) {
		HideCaret();
	} else {
		ShowCaret();
	}
	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
	int moveX = scrollInfo.nPos * horizontalScrollUnit;
	::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
	int moveY = scrollInfo.nPos * verticalScrollUnit;
	symbolSelector.SetCurrentPosition( GET_X_LPARAM( lParam ) + moveX, GET_Y_LPARAM( lParam ) + moveY );
	InvalidateRect( windowHandle, 0, true );
}

// private методы
// процедура обрабатывающая сообщения для окна редактора
LRESULT __stdcall CEditWindow::windowProcedure( HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_NCCREATE ) {
		::SetWindowLong( windowHandle, GWL_USERDATA, reinterpret_cast<LONG>( reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams ) );
	}

	CEditWindow* window = reinterpret_cast<CEditWindow*>( ::GetWindowLong( windowHandle, GWL_USERDATA ) );

	switch( message ) {
	case WM_CLOSE:
		window->OnWmDestroy();
		break;
	case WM_PAINT:
		window->OnWmPaint();
		break;
	case WM_HSCROLL:
		window->OnWmHScroll( wParam, lParam );
		break;
	case WM_VSCROLL:
		window->OnWmVScroll( wParam, lParam );
		break;
	case WM_SIZE:
		window->OnWmSize( lParam );
		break;
	case WM_LBUTTONDOWN:
		window->OnLButDown( lParam );
		break;
	case WM_MOUSEMOVE:
		if( wParam & MK_LBUTTON ) {
			window->OnLockedMouseMove( lParam );
		}
		break;
	case WM_LBUTTONUP:
		window->OnLButUp( lParam );
		break;
	}
	return ::DefWindowProc( windowHandle, message, wParam, lParam );

}

void CEditWindow::removeSelectedItems()
{
	CItemSelector::CSymbolInterval interval = symbolSelector.GetSelectionInfo();
	CSymbolPosition start = interval.first;
	CSymbolPosition end = interval.second;
	if( start.Index != -1 && end.Index != -1 ) {
		if( start.GetParent() != 0 ) {
			removeLocalSelected( start, end );
		} else {
			removeGlobalSelected( start, end );
		}
	}
}

void CEditWindow::removeLocalSelected( const CSymbolPosition& start, const CSymbolPosition& end )
{
	CLineOfSymbols* currentLine = const_cast<CLineOfSymbols*>( start.CurrentLine );
	for( int i = end.Index; i >= start.Index; --i ) {
		currentLine->Delete( i );
	}
	caret.MoveTo( CSymbolPosition( start.Index, start ) );
}

void CEditWindow::removeGlobalSelected( const CSymbolPosition& start, const CSymbolPosition& end )
{
	int startIdx = getBaseLineIndex( start.CurrentLine );
	int endIdx = getBaseLineIndex( end.CurrentLine );
	if( startIdx == endIdx ) {
		removeLocalSelected( start, end );
	} else {
		removeLocalSelected( start, CSymbolPosition( start.CurrentLine->Length() - 1, start ) );
		for( int i = startIdx + 1; i < endIdx; ++i ) {
			if( content[i].Length() != 0 ) {
				removeLocalSelected( CSymbolPosition( 0, &content[i] ), CSymbolPosition( content[i].Length() - 1, &content[i] ) );
			}
		}
		removeLocalSelected( CSymbolPosition( 0, end ), end );
		for( int i = 0; i < end.CurrentLine->Length(); ++i ) {
			content[startIdx].PushBack( content[endIdx][i]->Clone( &content[startIdx] ) );
		}
		content.erase( content.begin() + startIdx + 1, content.begin() + endIdx + 1 );
		caret.MoveTo( CSymbolPosition( start.Index, start ) );
	}
}

// проверяет, допустим ли данный символ
bool CEditWindow::isSymbolAllowed( wchar_t symbol ) const
{
	std::wstring allowedOperators = L"+-/*=^~()[]{}";
	return ( allowedOperators.find( symbol ) != std::wstring::npos || symbol == L' ' || ( symbol >= '0' && symbol <= '9' ) ||
		( symbol >= 'a'  &&  symbol <= 'z' ) || ( symbol >= 'A'  &&  symbol <= 'Z' ) );
}

// проверка, является CLineOFSYmbols одной из основных строк редактора
// если является - вернет индекс
// если нет - вернет -1
int CEditWindow::getBaseLineIndex( const CLineOfSymbols* line ) const
{
	for( int i = 0; i < static_cast<int>( content.size() ); ++i ) {
		if( line == &content[i] ) {
			return i;
		}
	}

	return -1;
}

// пересчитывает параметры горизонтального скролла
void CEditWindow::recalculateHorzScrollParams() const
{
	RECT clientRect;
	::GetClientRect( windowHandle, &clientRect );

	int width = 0;

	for( int i = 0; i < static_cast<int>( content.size() ); ++i ) {
		width = max( content[i].GetWidth(), width );
	}

	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
	scrollInfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;

	scrollInfo.nMax = width / horizontalScrollUnit;
	scrollInfo.nPage = clientRect.right / horizontalScrollUnit;
	if( scrollInfo.nPos > scrollInfo.nMax ) {
		scrollInfo.nPos = scrollInfo.nMax;
	}

	::SetScrollInfo( windowHandle, SB_HORZ, &scrollInfo, TRUE );
}

// пересчитывает параметры вертикального скролла
void CEditWindow::recalculateVertScrollParams() const
{
	RECT clientRect;
	::GetClientRect( windowHandle, &clientRect );

	int height = 0;

	for( int i = 0; i < static_cast<int>( content.size() ); ++i ) {
		height += content[i].GetHeight();
	}

	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
	scrollInfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;

	scrollInfo.nMax = height / verticalScrollUnit;
	scrollInfo.nPage = clientRect.bottom / verticalScrollUnit;
	if( scrollInfo.nPos > scrollInfo.nMax ) {
		scrollInfo.nPos = scrollInfo.nMax;
	}

	::SetScrollInfo( windowHandle, SB_VERT, &scrollInfo, TRUE );
}


// класс CCaret

CEditWindow::CCaret::CCaret( CEditWindow* _window, const CLineOfSymbols* baseLine ) : caretPosition( 0, baseLine )
{
	window = _window;
	shown = false;
	height = window->simpleSymbolHeight;
	width = 2;
}

void CEditWindow::CCaret::Create()
{
	::CreateCaret( window->windowHandle, 0, width, height );
	::SetCaretBlinkTime( 300 );
}

void CEditWindow::CCaret::Show()
{
	moveToNewCoordinates();
	if( !shown ) {
		shown = ::ShowCaret( window->windowHandle ) != 0;
	}
}

void CEditWindow::CCaret::Hide()
{
	if( shown ) {
		::HideCaret( window->windowHandle );
		shown = false;
	}
}

void CEditWindow::CCaret::Destroy()
{
	shown = false;
	::DestroyCaret();
}

void CEditWindow::CCaret::Move( CEditWindow::TCaretDirection direction )
{
	switch( direction ) {
	case CD_Up:
		moveUp();
		break;
	case CD_Down:
		moveDown();
		break;
	case CD_Left:
		moveLeft();
		break;
	case CD_Right:
		moveRight();
		break;
	}
}

void CEditWindow::CCaret::MoveTo( int x, int y )
{
	std::unique_ptr<const CSymbolPosition> tmp( window->finder.FindPosition( x, y ) );
	if( tmp->Index == -1 ) {
		MoveTo( CSymbolPosition( 0, *tmp ) );
	} else if( tmp->Index == tmp->CurrentLine->Length() ) {
		MoveTo( *tmp );
	} else {
		if( ( *tmp->CurrentLine )[tmp->Index]->GetX() + ( *tmp->CurrentLine )[tmp->Index]->GetWidth() / 2  > x ) {
			MoveTo( *tmp );
		} else {
			MoveTo( CSymbolPosition( tmp->Index + 1, *tmp ) );
		}
	}
}

void CEditWindow::CCaret::MoveTo( const CSymbolPosition& newPosition ) 
{
	CSymbolPosition tmp( newPosition );
	std::swap( caretPosition, tmp );
}

// этот код вынесен в отдельные методы
// для того чтобы функция CEditWindow::CCaret::Move не получилась слишком большой

// сдвигает каретку на единицу вверх
void CEditWindow::CCaret::moveUp()
{

}

// сдвигает каретку на единицу вниз
void CEditWindow::CCaret::moveDown()
{

}

// сдвигает каретку на единицу влево
void CEditWindow::CCaret::moveLeft()
{
	if( caretPosition.Index > 0 ) {
		--caretPosition.Index;
	} else if( int lineIndex = window->getBaseLineIndex( caretPosition.CurrentLine ) - 1 >= 0 ) {
		CSymbolPosition tmp( window->content[lineIndex].Length(), &window->content[lineIndex] );
		std::swap( caretPosition, tmp );
	}
	moveToNewCoordinates();
}

// сдвигает каретку на единицу в право
void CEditWindow::CCaret::moveRight()
{
	int lineIndex = window->getBaseLineIndex( caretPosition.CurrentLine );
	if( caretPosition.CurrentLine->Length() > caretPosition.Index ) {
		++caretPosition.Index;
	} else if( lineIndex >= 0 && lineIndex + 1 < static_cast<int>( window->content.size() ) ) {
		CSymbolPosition tmp( 0, &window->content[lineIndex + 1] );
		std::swap( caretPosition, tmp );
	}
	moveToNewCoordinates();

}

void CEditWindow::CCaret::moveToNewCoordinates()
{
	const CLineOfSymbols* caretLine = caretPosition.CurrentLine;
	int caretIndex = caretPosition.Index;
	int newHeight = caretLine->GetHeight();

	int x;
	int y = caretLine->GetY();
	if( caretLine->Length() == 0 ) {
		x = caretLine->GetX();
		newHeight = caretLine->GetHeight();
	} else if( caretIndex == 0 ) {
		x = ( *caretLine )[caretIndex]->GetX();
	} else {
		x = ( *caretLine )[caretIndex - 1]->GetX() + ( *caretLine )[caretIndex - 1]->GetWidth();
	}
	if( newHeight > 0 && newHeight != height ) {
		changeHeight( newHeight );
	}

	::SetCaretPos( x, y );
}

// меняет высоту каретки
void CEditWindow::CCaret::changeHeight( int newHeight )
{
	bool wasShown = shown;
	height = newHeight;
	Destroy();
	Create();
	if( wasShown ) {
		Show();
	}
}