#include "EditWindow.h"
#include "SimpleSymbol.h"
#include "FractionSymbol.h"
#include "SigmaSymbol.h"
#include "IndexSymbol.h"
#include <typeinfo>
#include "instruments.h"
#include <assert.h>
#include <windowsx.h>

// класс CEditWindow
// константы

const wchar_t* CEditWindow::className = L"MathRedactorEditWindowClass";

// public методы

CEditWindow::CEditWindow() 
		: horizontalScrollUnit( 30 )
		, verticalScrollUnit( 15 )
		, simpleSymbolHeight( 50 )
		, caret( this )
		, finder( content )
		, symbolSelector( finder )
		, drawer( content, symbolSelector, horizontalScrollUnit, verticalScrollUnit, simpleSymbolHeight )
{
	windowHandle = 0;
	
	content.push_back( CLineOfSymbols( simpleSymbolHeight ) );
	caret.MoveTo( &content[0], 0 );
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
	std::vector<CLineOfSymbols*> substrings;
	symbol->GetSubstrings( substrings );
	for( int i = 0; i < static_cast<int>( substrings.size() ); ++i ) {
		substrings[i]->SetParent( caret.GetLine() );
	}

	caret.GetLine()->Insert( caret.GetIndex(), symbol );
	caret.Move( CD_Right );
	recalculateHorzScrollParams();
	recalculateVertScrollParams();
	::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE | RDW_ERASE );
}

void CEditWindow::AddSign( wchar_t sign )
{
	if( symbolSelector.HasSelection() ) {
		// REMOVESELECTED
		symbolSelector.ResetSelection();
		if( !caret.IsShown() ) {
			ShowCaret();
		}
	}
	if( isSymbolAllowed( sign ) ) {
		AddSymbol( new CSimpleSymbol( sign ) );
	}
	::InvalidateRect( windowHandle, 0, true );
}

void CEditWindow::RemoveSign()
{
	if( caret.GetIndex() > 0 ) {
		caret.GetLine()->Delete( caret.GetIndex() - 1 );
		caret.Move( CD_Left );
	} else if( getBaseLineIndex( caret.GetLine() ) > 0 ) {
		int lineIndex = getBaseLineIndex( caret.GetLine() );
		int pos = content[lineIndex - 1].Length();
		for( int i = 0; i < content[lineIndex].Length(); ++i ) {
			content[lineIndex - 1].PushBack( content[lineIndex][i]->Clone( &content[lineIndex - 1] ) );
		}
		content.erase( content.begin() + lineIndex );
		caret.MoveTo( &content[lineIndex - 1], pos );
		recalculateVertScrollParams();
	}
	recalculateHorzScrollParams();
	::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
}

void CEditWindow::NewLine()
{
	int lineIndex = getBaseLineIndex( caret.GetLine() );
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
		caret.MoveTo( &content[lineIndex + 1], 0 );
		recalculateVertScrollParams();
		::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
	}
}

void CEditWindow::ShowCaret()
{
	caret.Create();
	caret.Show();
}

void CEditWindow::HideCaret()
{
	caret.Hide();
	caret.Destroy();
}

void CEditWindow::MoveCaret( CEditWindow::TCaretDirection direction )
{
	if( symbolSelector.HasSelection() ) {
		// TODO SMTH
		int begin = 0, end = 0;
		CLineOfSymbols* newLine = caret.GetLine();
		symbolSelector.ResetSelection();
		caret.MoveTo( newLine, begin );
		::InvalidateRect( windowHandle, 0, true );
	}
	caret.Move( direction );
	caret.Show();
}

void CEditWindow::MoveCaretTo( int x, int y )
{
	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
	int movedX = scrollInfo.nPos * horizontalScrollUnit + x;
	::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
	int movedY = scrollInfo.nPos * verticalScrollUnit + y;
	int lineIdx = 0;
	int symbolIdx = 0;

	int currentY = 0;
	for( lineIdx = 0; lineIdx < static_cast<int>( content.size() ); ++lineIdx ) {
		currentY += content[lineIdx].GetHeight();
		if( currentY >= movedY ) {
			break;
		}
	}
	if( currentY < movedY ) {
		caret.MoveTo( &content[content.size() - 1], content[content.size() - 1].Length() );
		return;
	}
	int currentX = 0;
	for( symbolIdx = 0; symbolIdx < content[lineIdx].Length(); ++symbolIdx ) {
		currentX += content[lineIdx][symbolIdx]->GetWidth();
		if( currentX >= movedX ) {
			break;
		} 
	}

	CLineOfSymbols* baseLine;

	// Если выделение начинается за границей существующих строк / символов в строке, то выделяем по всему документу
	if( lineIdx == content.size() ) {
		baseLine = &content[--lineIdx];
	} else {
		baseLine = isLineBase( &content[lineIdx], x, y );
	}
	if( baseLine->Length() == 0 ) {
		symbolIdx = 0;
	} else {
		currentX = baseLine->GetX();
		for( symbolIdx = 0; symbolIdx < baseLine->Length(); ++symbolIdx ) {
			currentX += (*baseLine)[symbolIdx]->GetWidth();
			if( currentX > x ) {
				break;
			}
		}
		if( symbolIdx < baseLine->Length() ) {
			currentX -= static_cast<int>( ( (*baseLine)[symbolIdx]->GetWidth() / 1.5 ) );
			if( currentX < x ) {
				++symbolIdx;
			}
		}
	}
	caret.MoveTo( baseLine, symbolIdx );
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

	MoveCaretTo( x, y );
	symbolSelector.ResetSelection();
	caret.Show();
	symbolSelector.SetStartPosition( x + moveX, y + moveY );
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
		caret.Show();
		MoveCaretTo( x, y );
	}
	InvalidateRect( windowHandle, 0, true );
}

void CEditWindow::OnLockedMouseMove( LPARAM lParam )
{
	caret.Hide();
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
	default:
		return ::DefWindowProc( windowHandle, message, wParam, lParam );
	}

}

CLineOfSymbols* CEditWindow::isLineBase( CLineOfSymbols* currentBaseLine, int x, int y )
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
	// если символ простой или мы ушли за границу строки, то текущая линия уже является стартовой для выделения
	std::vector<CLineOfSymbols*> substings;
	// если символ простой или мы ушли за границу строки, то текущая линия уже является стартовой для выделения
	if( currentX < x || currentBaseLine->Length() == 0) {
		return currentBaseLine;
	// иначе в зависимости от типа символа смотрим, не нужно ли перейти на одну из "более внутренних" строк
	} else {
		(*currentBaseLine)[symbolIdx]->GetSubstrings( substings );
		if( substings.size() == 0 ) {
			return currentBaseLine;
		} else {
			for( int i = 0; i < static_cast<int>( substings.size() ); ++i ) {
				if( IsLineContainPoint( substings[i], x, y ) ) {
					return isLineBase( substings[i], x, y );
				}
			}
			return currentBaseLine;
		}
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

CEditWindow::CCaret::CCaret( CEditWindow* _window )
{
	window = _window;
	line = 0;
	index = 0;
	shown = false;
	height = window->simpleSymbolHeight;
	width = 2;
}

CLineOfSymbols* CEditWindow::CCaret::GetLine()
{
	return line;
}

int CEditWindow::CCaret::GetIndex() const
{
	return index;
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

void CEditWindow::CCaret::MoveTo( CLineOfSymbols* _line, int _index ) 
{
	line = _line;
	index = _index;
}

bool CEditWindow::CCaret::IsShown() const
{
	return shown;
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
	if( index > 0 ) {
		--index;
		moveToNewCoordinates();
	} else if( window->getBaseLineIndex( line ) > 0 ) {
		int lineIndex = window->getBaseLineIndex( line ) - 1;
		line = &( window->content[lineIndex] );
		index = line->Length();
		moveToNewCoordinates();
	}
}

// сдвигает каретку на единицу в право
void CEditWindow::CCaret::moveRight()
{
	int lineIndex = window->getBaseLineIndex( line );
	if( line->Length() > index ) {
		++index;
		moveToNewCoordinates();
	} else if( lineIndex >= 0 && lineIndex + 1 < static_cast<int>( window->content.size() ) ) {
		line = &( window->content[lineIndex + 1] );
		index = 0;
		moveToNewCoordinates();
	}
}

void CEditWindow::CCaret::moveToNewCoordinates()
{
	int x;
	int newHeight = line->GetHeight();
	int y = line->GetY();
	if( line->Length() == 0 ) {
		x = line->GetX();
		newHeight = line->GetHeight();
	} else if( index == 0 ) {
		x = ( *line )[index]->GetX();
	} else {
		x = ( *line )[index - 1]->GetX() + ( *line )[index - 1]->GetWidth();
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