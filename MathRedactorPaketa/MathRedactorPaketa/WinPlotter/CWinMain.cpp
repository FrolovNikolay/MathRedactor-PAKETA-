/*
author: Timur Khusaenov
class: CWinMain
description:
	Класс реализует интерфейс, применяемый в приложении WinPlotter.
	Для корректной работы должен содержвать делегата CWinPlotter, который реализует отрисовку графика
*/

#pragma once
#include "evaluate.h"
const int indentFromBorder = 25;

#include "CWinMain.h"
#include <Windows.h>
#include "resource.h"
#include <assert.h>
#include <cmath>

WNDPROC CWinMain::defButtonProc = 0;
WNDPROC CWinMain::defMouseProc = 0;

bool CWinMain::registerClass( HINSTANCE hInstance )
{
	WNDCLASSEX windowClass;
	::ZeroMemory( &windowClass, sizeof( WNDCLASSEX ) );
	windowClass.cbSize = sizeof( WNDCLASSEX );
	windowClass.style = CS_GLOBALCLASS | CS_VREDRAW | CS_HREDRAW;
	windowClass.lpfnWndProc = CWinMain::windowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = L"CWinMain";
	windowClass.lpszMenuName = MAKEINTRESOURCE( IDR_MENU );

	ATOM res = ::RegisterClassEx( &windowClass );

	return ( res != 0 );
}

HWND CWinMain::create( HINSTANCE hInstance )
{
	handle = ::CreateWindow( L"CWinMain", L"CWinMain", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInstance, this );
	return handle;
}

void CWinMain::show( int cmdShow )
{
	::ShowWindow( handle, cmdShow );
}

HWND CWinMain::createButton( LPCWSTR title, int X, int Y, HWND parent, HMENU id )
{
	static bool init = false;
	HINSTANCE hInstance = reinterpret_cast< HINSTANCE >( GetWindowLong( parent, GWL_HINSTANCE ) );
	HWND handle = CreateWindow( L"BUTTON", title, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, X, Y, buttonSize, buttonSize, parent, ( HMENU )ID_BUTTON_MOVE_BOT, hInstance, 0 );
	if( init == false ) {
		CWinMain::defButtonProc = ( WNDPROC )SetWindowLong( handle, GWL_WNDPROC, ( LONG )CWinMain::buttonProc );
		init = true;
	} else {
		SetWindowLong( handle, GWL_WNDPROC, ( LONG )CWinMain::buttonProc );
	}
	return handle;
}

/* 
обертка над дефолтным обработчиком нажатия кнопок
*/
LRESULT __stdcall CWinMain::buttonProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HWND parent = GetParent( hWnd );
	CWinMain* wnd = reinterpret_cast<CWinMain*>( GetWindowLong( parent, GWL_USERDATA ) );
	switch( uMsg ) {
		case WM_LBUTTONDOWN:
			wnd->IdentifyCommand( hWnd );
			break;
		case WM_LBUTTONUP:
			wnd->curMove = false;
			wnd->moveDirection = D_None;
			wnd->rotateDirection = D_None;
			wnd->zoom = Z_None;
			break;
			// assert не нужен, так как обертка
	}
	return CallWindowProc( defButtonProc, hWnd, uMsg, wParam, lParam );
}

/*
обертка над оброботчиком дочернего окна 
*/
LRESULT __stdcall CWinMain::mouseProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HWND parent = GetParent( hWnd );
	CWinMain* wnd = reinterpret_cast<CWinMain*>( GetWindowLong( parent, GWL_USERDATA ) );
	switch( uMsg ) {
		case WM_LBUTTONDOWN:
			SetCapture( hWnd );
			GetCursorPos( &(wnd->oldCurPos) );
			wnd->curMove = true;
			break;
		case WM_LBUTTONUP:
			ReleaseCapture();
			wnd->curMove = false;
			break;
			// assert не нужен, так как обертка
	}
	return CallWindowProc( defMouseProc, hWnd, uMsg, wParam, lParam );
}

void CWinMain::IdentifyCommand( HWND hWnd )
{
	if( hWnd == hButtonMoveBot ) {
		moveDirection = D_Bot;
	} else if( hWnd == hButtonMoveTop ) {
		moveDirection = D_Top;
	} else if( hWnd == hButtonMoveRight ) {
		moveDirection = D_Right;
	} else if( hWnd == hButtonMoveLeft ) {
		moveDirection = D_Left;
	} else if( hWnd == hButtonRotateDown ) {
		rotateDirection = D_Bot;
	} else if( hWnd == hButtonRotateUp ) {
		rotateDirection = D_Top;
	} else if( hWnd == hButtonRotateLeft ) {
		rotateDirection = D_Left;
	} else if( hWnd == hButtonRotateRight ) {
		rotateDirection = D_Right;
	} else if( hWnd == hButtonZoomMinus ) {
		zoom = Z_Minus;
	} else if( hWnd == hButtonZoomPlus ) {
		zoom = Z_Plus;
	} else {
		assert( false );
	}
}

void CWinMain::setButtonPos( HWND hWnd, int X, int Y )
{
	SetWindowPos( hWnd, HWND_TOP, X, Y, buttonSize, buttonSize, SWP_NOOWNERZORDER );
}

/*
обработчик главного окна
*/
LRESULT __stdcall CWinMain::windowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_NCCREATE ) {
		SetWindowLong( hWnd, GWL_USERDATA, reinterpret_cast<LONG>( ( reinterpret_cast<CREATESTRUCT*>( lParam ) )->lpCreateParams ) );
	}
	CWinMain* wnd = reinterpret_cast<CWinMain*>( GetWindowLong( hWnd, GWL_USERDATA ) );

	switch( uMsg ) {
		case WM_NCCREATE:
			wnd->timer = SetTimer( hWnd, 0, 10, 0 );
			break;
		case WM_CREATE:
			wnd->OnCreate( hWnd );
			return 0;
		case WM_SIZE:
			wnd->ResizeChildrens();
			return 0;
		case WM_DESTROY:
			wnd->OnDestroy();
			return 0;
		case WM_TIMER:
			wnd->Move();
			return 0;
		case WM_COMMAND:
			return wnd->OnCommand( wParam, lParam );
		case WM_KEYDOWN:
			return wnd->OnKeyDown( wParam, lParam );
		case WM_KEYUP:
			return wnd->OnKeyUp( wParam, lParam );
		case WM_MOUSEWHEEL:
			wnd->winPlotter.zoom( GET_WHEEL_DELTA_WPARAM( wParam ) * wnd->MouseWheelSens );
			return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

void CWinMain::Move()
{
	switch( moveDirection ) {
		case D_None:
			break;
		case D_Top:
			winPlotter.moveY( -1 );
			break;
		case D_Bot:
			winPlotter.moveY();
			break;
		case D_Left:
			winPlotter.moveX( -1 );
			break;
		case D_Right:
			winPlotter.moveX();
			break;
	}

	switch( rotateDirection ) {
		case D_None:
			break;
		case D_Top:
			winPlotter.rotateY( -1 );
			break;
		case D_Bot:
			winPlotter.rotateY();
			break;
		case D_Left:
			winPlotter.rotateX( -1 );
			break;
		case D_Right:
			winPlotter.rotateX();
			break;
	}

	switch( zoom ) {
		case Z_None:
			break;
		case Z_Plus:
			winPlotter.zoom();
			break;
		case Z_Minus:
			winPlotter.zoom( -1 );
			break;
	}
	if( curMove ) {
		GetCursorPos( &curPos );
		winPlotter.rotateX( oldCurPos.x - curPos.x );
		winPlotter.rotateY( oldCurPos.y - curPos.y );
		oldCurPos = curPos;
	}
}

void CWinMain::OnDestroy() {
	KillTimer( handle, timer );
	::PostQuitMessage( 0 );
}

void CWinMain::OnCreate( HWND hWnd ) {
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>( GetWindowLong( hWnd, GWL_HINSTANCE ) );
	
	RECT rect;
	GetClientRect( hWnd, &rect );
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	hChild = winPlotter.create( hInstance, hWnd );
	
	if ( hChild == 0 ) {
		MessageBox( 0, L"Не удалось создать дочернее окно", L"Ошибка", MB_OK | MB_ICONERROR );
		OnDestroy();
	}
	winPlotter.show( SW_SHOW );
	SetWindowPos( hChild, HWND_BOTTOM, 0, 0, width, height, 0 );
	CWinMain::defMouseProc = ( WNDPROC )SetWindowLong( hChild, GWL_WNDPROC, ( LONG )CWinMain::mouseProc );

	int buttonBlockPosX = buttonSize + indentFromBorder;				// отступ центральной кнопки равен размеру кнопки + 25
	int buttonBlockPosY = height - buttonSize - indentFromBorder;		// отступ центральной кнопки равен размеру кнопки + 25

	hButtonMoveBot = createButton( L".", buttonBlockPosX, buttonBlockPosY, hWnd, ( HMENU )ID_BUTTON_MOVE_BOT );
	hButtonMoveTop = createButton( L"^", buttonBlockPosX, buttonBlockPosY - buttonSize, hWnd, ( HMENU )ID_BUTTON_MOVE_TOP );
	hButtonMoveLeft = createButton( L"<", buttonBlockPosX - buttonSize, buttonBlockPosY, hWnd, ( HMENU )ID_BUTTON_MOVE_LEFT );
	hButtonMoveRight = createButton( L">", buttonBlockPosX + buttonSize, buttonBlockPosY, hWnd, ( HMENU )ID_BUTTON_MOVE_RIGHT );

	buttonBlockPosX = width - 2 * buttonSize - indentFromBorder;			// 2-ой отступ + 25 так как положение кнопки считаетс¤ от левого верхнего угла
	// buttonBlockPosY = height - buttonSize - indentFromBorder;

	hButtonRotateDown = createButton( L".", buttonBlockPosX, buttonBlockPosY, hWnd, ( HMENU )ID_BUTTON_ROTATE_DOWN );
	hButtonRotateUp = createButton( L"^", buttonBlockPosX, buttonBlockPosY - buttonSize, hWnd, ( HMENU )ID_BUTTON_ROTATE_UP );
	hButtonRotateLeft = createButton( L"<", buttonBlockPosX - buttonSize, buttonBlockPosY, hWnd, ( HMENU )ID_BUTTON_ROTATE_LEFT );
	hButtonRotateRight = createButton( L">", buttonBlockPosX + buttonSize, buttonBlockPosY, hWnd, ( HMENU )ID_BUTTON_ROTATE_RIGHT );

	buttonBlockPosX = width - buttonSize - indentFromBorder;
	buttonBlockPosY = buttonSize + indentFromBorder;

	hButtonZoomMinus = createButton( L".", buttonBlockPosX, buttonBlockPosY, hWnd, ( HMENU )ID_BUTTON_ZOOM_OUT );
	hButtonZoomPlus = createButton( L"^", buttonBlockPosX, buttonBlockPosY - buttonSize, hWnd, ( HMENU )ID_BUTTON_ZOOM_ON );
}

void CWinMain::ResizeChildrens() {
	RECT rect;
	GetClientRect( handle, &rect );
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	SetWindowPos( hChild, HWND_TOP, 0, 0, width, height, SWP_NOOWNERZORDER );

	int buttonBlockPosX = buttonSize + indentFromBorder;				// отступ центральной кнопки равен размеру кнопки + 25
	int buttonBlockPosY = height - buttonSize - indentFromBorder;		// отступ центральной кнопки равен размеру кнопки + 25

	setButtonPos( hButtonMoveBot, buttonBlockPosX, buttonBlockPosY );
	setButtonPos( hButtonMoveTop, buttonBlockPosX, buttonBlockPosY - buttonSize );
	setButtonPos( hButtonMoveLeft, buttonBlockPosX - buttonSize, buttonBlockPosY );
	setButtonPos( hButtonMoveRight, buttonBlockPosX + buttonSize, buttonBlockPosY );

	buttonBlockPosX = width - 2 * buttonSize - indentFromBorder;
	// buttonBlockPosY = height - buttonSize - indentFromBorder;

	setButtonPos( hButtonRotateDown, buttonBlockPosX, buttonBlockPosY );
	setButtonPos( hButtonRotateUp, buttonBlockPosX, buttonBlockPosY -buttonSize );
	setButtonPos( hButtonRotateLeft, buttonBlockPosX - buttonSize, buttonBlockPosY );
	setButtonPos( hButtonRotateRight, buttonBlockPosX + buttonSize, buttonBlockPosY );

	buttonBlockPosX = width - buttonSize - indentFromBorder;
	buttonBlockPosY = buttonSize + indentFromBorder;

	setButtonPos( hButtonZoomMinus, buttonBlockPosX, buttonBlockPosY );
	setButtonPos( hButtonZoomPlus, buttonBlockPosX, buttonBlockPosY - buttonSize );
}

LRESULT CWinMain::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int wmId = LOWORD( wParam );

	switch( wmId ) {
		case ID_NEWFORMULA:
			ShowFormulaForm();
			break;
		case ID_CLEAR:
			winPlotter.clear();
			break;
	}
	return DefWindowProc( handle, WM_COMMAND, wParam, lParam );
}

void CWinMain::ShowFormulaForm()
{
	if( hFormulaForm == 0 ) {
		hFormulaForm = ::CreateDialog( 0, MAKEINTRESOURCE( IDD_FORMULA_FORM ), handle, formulaDialogProc );
		ShowWindow( hFormulaForm, SW_SHOW );
	}
}

/*
обработчик диалогового окна ввода формулы
*/
BOOL __stdcall CWinMain::formulaDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CWinMain* wnd = reinterpret_cast<CWinMain*>( GetWindowLong( ::GetParent( hWnd ), GWL_USERDATA ) );

	switch( uMsg ) {
		case WM_COMMAND:
			return wnd->OnFormCommand( wParam, lParam );
		case WM_CLOSE:
			EndDialog( hWnd, 0 );
			DestroyWindow( hWnd );
			wnd->hFormulaForm = 0;
			return TRUE;
		default:
			return FALSE;
	}
}

LRESULT CWinMain::OnFormCommand( WPARAM wParam, LPARAM lParam )
{
	int wmId = LOWORD( wParam );

	switch( wmId ) {
		case IDOK:
			TakeFormula();
			EndDialog( hFormulaForm, 0 );
			DestroyWindow( hFormulaForm );
			hFormulaForm = 0;
			return TRUE;
		case IDCANCEL:
			EndDialog( hFormulaForm, 0 );
			DestroyWindow( hFormulaForm );
			hFormulaForm = 0;
			return TRUE;
		default:
			return FALSE;
	}
}

void CWinMain::TakeFormula()
{
	size_t i;
	LRESULT textLength = ::SendDlgItemMessage( hFormulaForm, IDC_EDIT_FORM, WM_GETTEXTLENGTH, 0, 0 );
	TCHAR* buff = new TCHAR[textLength + 1];
	char* outbuff = new char[textLength + 1];
	::SendDlgItemMessage( hFormulaForm, IDC_EDIT_FORM, WM_GETTEXT, textLength + 1, ( LPARAM )buff );
	wcstombs_s( &i, outbuff, textLength + 1, buff, textLength + 1 );
	std::string data( outbuff );
	CGraphBuilder builder;
	if( !builder.buildPointGrid( data ) ) {
		::MessageBox( hFormulaForm, L"Formula builder error", L"Error", MB_OK );
	} else {
		winPlotter.testObject.Clear();
		const std::vector< C3DPoint >& points = builder.GetPoints();
		const std::vector< std::pair< int, int > >& segmentsIds = builder.GetSegments();
		for( int j = 0; j < points.size(); j++ ) {
			winPlotter.testObject.AddPoint( points[j] );
		}
		for( int j = 0; j < segmentsIds.size(); j++ ) {
			winPlotter.testObject.AddSegment( segmentsIds[j].first, segmentsIds[j].second );
		}
		winPlotter.moveX( 0 );
	}

	delete[] buff;
	delete[] outbuff;
}

LRESULT CWinMain::OnKeyDown( WPARAM wParam, LPARAM lParam )
{
	switch( wParam ) {
		case VK_LEFT:
			moveDirection = D_Left;
			return 0;
		case VK_RIGHT:
			moveDirection = D_Right;
			return 0;
		case VK_UP:
			moveDirection = D_Top;
			return 0;
		case VK_DOWN:
			moveDirection = D_Bot;
			return 0;
	}
	return DefWindowProc( handle, WM_KEYDOWN, wParam, lParam );
}

LRESULT CWinMain::OnKeyUp( WPARAM wParam, LPARAM lParam )
{
	switch( wParam ) {
		case VK_LEFT:
			if( moveDirection == D_Left ) {
				moveDirection = D_None;
			}
			return 0;
		case VK_RIGHT:
			if( moveDirection == D_Right ) {
				moveDirection = D_None;
			}
			return 0;
		case VK_UP:
			if( moveDirection == D_Top ) {
				moveDirection = D_None;
			}
			return 0;
		case VK_DOWN:
			if( moveDirection == D_Bot ) {
				moveDirection = D_None;
			}
			return 0;
	}
	return DefWindowProc( handle, WM_KEYDOWN, wParam, lParam );
}