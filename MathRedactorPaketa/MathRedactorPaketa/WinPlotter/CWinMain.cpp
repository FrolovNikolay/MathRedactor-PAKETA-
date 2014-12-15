/*
author: Timur Khusaenov
class: CWinMain
description:
	Класс реализует интерфейс, применяемый в приложении WinPlotter.
	Для корректной работы должен содержвать делегата CWinPlotter, который реализует отрисовку графика
*/

#pragma once
#include "evaluate.h"
#include "MainWindow.h"
#include <Commctrl.h>
#include "Messages.h"
#include "Utils.h"
#include "CWinMain.h"
#include <Windows.h>
#include "resource.h"
#include <assert.h>
#include <cmath>

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
	DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	handle = ::CreateWindow( L"CWinMain", L"Редактор формул -РАКЕТА-", style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInstance, this );
	return handle;
}

void CWinMain::show( int cmdShow )
{
	::ShowWindow( handle, cmdShow );
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
		case WM_REDACTOR_OK:
			wnd->TakeFormula();
			return 0;
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
			wnd->winPlotter.zoom( static_cast<LONG>( GET_WHEEL_DELTA_WPARAM( wParam ) ) );
			return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
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

	hPlotter = winPlotter.create( hInstance, hWnd );
	hRedactor = winRedactor.Create( hInstance, hWnd );
    
	
	if ( hPlotter == 0 || hRedactor == 0 ) {
		MessageBox( 0, L"Не удалось создать дочернее окно", L"Ошибка", MB_OK | MB_ICONERROR );
		OnDestroy();
	}
	winPlotter.show( SW_SHOW );
	winRedactor.Show( SW_HIDE );

	SetWindowPos( hPlotter, HWND_BOTTOM, 0, 0, width, height, 0 );
	SetWindowPos( hRedactor, HWND_BOTTOM, 0, 0, width, height, 0 );

	CWinMain::defMouseProc = ( WNDPROC )SetWindowLong( hPlotter, GWL_WNDPROC, ( LONG )CWinMain::mouseProc );
}

void CWinMain::ResizeChildrens() {
	RECT rect;
	GetClientRect( handle, &rect );
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	SetWindowPos( hPlotter, HWND_TOP, 0, 0, width, height, SWP_NOOWNERZORDER );
	SetWindowPos( hRedactor, HWND_BOTTOM, 0, 0, width, height, SWP_NOOWNERZORDER );
}

LRESULT CWinMain::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int wmId = LOWORD( wParam );

	switch( wmId ) {
		case ID_NEWFORMULA:
			ShowFormulaForm();
			break;
		case ID_PARAMS:
			ShowParamForm();
			break;
        case ID_CMD_COPY:
            if( ::IsWindowVisible( hRedactor ) ) {
                winRedactor.Copy();
            }
            break;
        case ID_CMD_PASTE:
            if( ::IsWindowVisible( hRedactor ) ) {
                winRedactor.Paste();
            }
	}
	return DefWindowProc( handle, WM_COMMAND, wParam, lParam );
}

void CWinMain::ShowFormulaForm()
{
	winRedactor.Show( SW_SHOW );
	::EnableWindow( handle, false );
}

void CWinMain::ShowParamForm()
{
	if( hFormulaForm == 0 ) {
		hFormulaForm = ::CreateDialog( 0, MAKEINTRESOURCE( IDD_FORMULA_FORM ), handle, formulaDialogProc );
		::ShowWindow( hFormulaForm, SW_SHOW );
	}
}

/*
обработчик диалогового окна ввода параметров
*/
BOOL __stdcall CWinMain::formulaDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CWinMain* wnd = reinterpret_cast<CWinMain*>( GetWindowLong( ::GetParent( hWnd ), GWL_USERDATA ) );

	switch( uMsg ) {
		case WM_INITDIALOG:
			wnd->OnDialogCreate( hWnd );
			return TRUE;
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

void CWinMain::OnDialogCreate( HWND hWnd )
{
	WinPlotter::SetValue( hWnd, IDC_EDIT_MAX_PARAM_1, maxParam[0] );
	WinPlotter::SetValue( hWnd, IDC_EDIT_MAX_PARAM_2, maxParam[1] );
	WinPlotter::SetValue( hWnd, IDC_EDIT_MIN_PARAM_1, minParam[0] );
	WinPlotter::SetValue( hWnd, IDC_EDIT_MIN_PARAM_2, minParam[1] );
	WinPlotter::SetValue( hWnd, IDC_EDIT_EPS, epsilon );

	WinPlotter::SetParamText( hWnd, IDC_STATIC_PARAM_1, vars[0] );

	if( vars.size() == 1 ) {
		WinPlotter::HideEdit( hWnd, IDC_EDIT_MAX_PARAM_2 );
		WinPlotter::HideEdit( hWnd, IDC_EDIT_MIN_PARAM_2 );
		WinPlotter::HideEdit( hWnd, IDC_STATIC_PARAM_2 );
		WinPlotter::HideEdit( hWnd, IDC_STATIC_MIN2 );
		WinPlotter::HideEdit( hWnd, IDC_STATIC_MAX2 );
	}
	if( vars.size() == 2 ) {
		WinPlotter::SetParamText( hWnd, IDC_STATIC_PARAM_2, vars[1] );
	}
}

LRESULT CWinMain::OnFormCommand( WPARAM wParam, LPARAM lParam )
{
	int wmId = LOWORD( wParam );

	switch( wmId ) {
		case IDOK:
			OnFormOk();
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

void CWinMain::OnFormOk()
{
	TCHAR buff[150];
	TCHAR* stopString;
	double tempMax_1, tempMin_1, tempMax_2, tempMin_2, temp_eps;

	::GetDlgItemText( hFormulaForm, IDC_EDIT_MAX_PARAM_1, buff, 150 );
	tempMax_1 = wcstod( buff, &stopString );
	if( wcslen( stopString ) > 0 ) {
		::MessageBox( hFormulaForm, L"Введен некорректный параметр для максимального значения 1-ого параметра", L"Error", MB_OK | MB_ICONERROR );
		return;
	}
	::GetDlgItemText( hFormulaForm, IDC_EDIT_MAX_PARAM_2, buff, 150 );
	tempMax_2 = wcstod( buff, &stopString );
	if( wcslen( stopString ) > 0 ) {
		::MessageBox( hFormulaForm, L"Введен некорректный параметр для максимального значения 2-ого параметра", L"Error", MB_OK | MB_ICONERROR );
		return;
	}
	::GetDlgItemText( hFormulaForm, IDC_EDIT_MIN_PARAM_1, buff, 150 );
	tempMin_1 = wcstod( buff, &stopString );
	if( wcslen( stopString ) > 0 ) {
		::MessageBox( hFormulaForm, L"Введен некорректный параметр для минимального значения 1-ого параметра", L"Error", MB_OK | MB_ICONERROR );
		return;
	}
	::GetDlgItemText( hFormulaForm, IDC_EDIT_MIN_PARAM_2, buff, 150 );
	tempMin_2 = wcstod( buff, &stopString );
	if( wcslen( stopString ) > 0 ) {
		::MessageBox( hFormulaForm, L"Введен некорректный параметр для минимального значения 2-ого параметра", L"Error", MB_OK | MB_ICONERROR );
		return;
	}
	::GetDlgItemText( hFormulaForm, IDC_EDIT_EPS, buff, 150 );
	temp_eps = wcstod( buff, &stopString );
	if( wcslen( stopString ) > 0 ) {
		::MessageBox( hFormulaForm, L"Введен некорректный параметр значения эпсилон", L"Error", MB_OK | MB_ICONERROR );
		return;
	}
	if( tempMax_1 < tempMin_1 || tempMax_2 < tempMin_2 ) {
		::MessageBox( hFormulaForm, L"Максимум меньше минимума", L"Error", MB_OK | MB_ICONERROR );
		return;
	}

	maxParam[0] = tempMax_1;
	minParam[0] = tempMin_1;
	maxParam[1] = tempMax_2;
	minParam[1] = tempMin_2;
	epsilon = temp_eps;

	args.clear();

	for( int i = 0; i < static_cast<int>( vars.size() ); i++ ) {
		args[vars[i]] = std::make_pair( minParam[i], maxParam[i] );
	}
	buildPlot();
	EndDialog( hFormulaForm, 0 );
	DestroyWindow( hFormulaForm );
	hFormulaForm = 0;
}

void CWinMain::TakeFormula()
{
	formula = ParseFormula( winRedactor.CalculateStringForPlotter() );
	vars = formula.GetVariables();

	args.clear();
	for( int i = 0; i < static_cast<int>( vars.size() ); i++ ) {
		args[vars[i]] = std::make_pair( minParam[i], maxParam[i] );
	}
	buildPlot();
	::EnableMenuItem( GetMenu( handle ), ID_PARAMS, MF_ENABLED );
}

void CWinMain::buildPlot()
{
	CGraphBuilder builder;
	if( !builder.buildPointGrid( formula, args, epsilon ) ) {
		::MessageBox( hFormulaForm, L"Formula builder error", L"Error", MB_OK | MB_ICONERROR );
	} else {
		winPlotter.testObject.Clear();

		const std::vector< C3DPoint >& points = builder.GetPoints();
		const std::vector< std::pair< int, int > >& segmentsIds = builder.GetSegments();
		const std::vector< CTriangleIndex >& triangles = builder.GetTriangles();

		for( int j = 0; j < static_cast< int >( points.size() ); j++ ) {
			winPlotter.testObject.AddPoint( points[j] );
		}
		for( int j = 0; j < static_cast< int >( segmentsIds.size() ); j++ ) {
			winPlotter.testObject.AddSegment( segmentsIds[j].first, segmentsIds[j].second );
		}
		for( int j = 0; j < static_cast< int >( triangles.size() ); j++ ) {
			winPlotter.testObject.AddTriangle( triangles[j].First, triangles[j].Second, triangles[j].Third );
		}
		winPlotter.maxZ = builder.GetMaxZ();
		winPlotter.minZ = builder.GetMinZ();

		winPlotter.moveX( 0 );
	}
}

LRESULT CWinMain::OnKeyDown( WPARAM wParam, LPARAM lParam )
{
	switch( wParam ) {
		case VK_LEFT:
			rotation = D_Left;
			return 0;
		case VK_RIGHT:
			rotation = D_Right;
			return 0;
		case VK_UP:
			rotation = D_Top;
			return 0;
		case VK_DOWN:
			rotation = D_Bot;
			return 0;
		case VK_SPACE:
			winPlotter.reset();
			return 0;
		default:
			return DefWindowProc( handle, WM_KEYDOWN, wParam, lParam );
	}
}

LRESULT CWinMain::OnKeyUp( WPARAM wParam, LPARAM lParam )
{
	switch( wParam ) {
		case VK_LEFT:
			if( rotation == D_Left ) {
				rotation = D_None;
			}
			return 0;
		case VK_RIGHT:
			if( rotation == D_Right ) {
				rotation = D_None;
			}
			return 0;
		case VK_UP:
			if( rotation == D_Top ) {
				rotation = D_None;
			}
			return 0;
		case VK_DOWN:
			if( rotation == D_Bot ) {
				rotation = D_None;
			}
			return 0;
	}
	return DefWindowProc( handle, WM_KEYDOWN, wParam, lParam );
}

void CWinMain::Move()
{
	switch( rotation ) {
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
		default:
			assert( false );
	}

	if( curMove ) {
		GetCursorPos( &curPos );
		winPlotter.moveX( oldCurPos.x - curPos.x );
		winPlotter.moveY( oldCurPos.y - curPos.y );
		oldCurPos = curPos;
	}
}