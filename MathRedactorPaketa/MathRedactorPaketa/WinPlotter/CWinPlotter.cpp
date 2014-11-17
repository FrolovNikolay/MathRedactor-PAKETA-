#include "CWinPlotter.h"
#include <Windows.h>
#include "resource.h"

#include "3DPoint.h"

LRESULT __stdcall CWinPlotter::windowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_NCCREATE ) {
		SetWindowLong( hWnd, GWL_USERDATA, reinterpret_cast< LONG >( ( reinterpret_cast< CREATESTRUCT* >( lParam ) )->lpCreateParams ) );
	}
	CWinPlotter* wnd = reinterpret_cast< CWinPlotter* >( GetWindowLong( hWnd, GWL_USERDATA ) );
	switch( uMsg ) {
		case WM_CREATE:
			wnd->OnCreate();
			return 0;
		case WM_SIZE:
			wnd->Invalidate();
			return 0;
		case WM_PAINT:
			wnd->PaintObject();
			return 0;
		case WM_DESTROY:
			wnd->OnDestroy();
			break;
	}
	return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
}

bool CWinPlotter::registerClass( HINSTANCE hInstance )
{
	WNDCLASSEX windowClass;
	::ZeroMemory( &windowClass, sizeof( WNDCLASSEX ) );
	windowClass.cbSize = sizeof( WNDCLASSEX );
	windowClass.style = CS_VREDRAW | CS_HREDRAW;
	windowClass.lpfnWndProc = CWinPlotter::windowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = L"CWinPlotter";

	ATOM res = ::RegisterClassEx( &windowClass );
	return ( res != 0 );
}

HWND CWinPlotter::create( HINSTANCE hInstance, HWND parent )
{
	handle = ::CreateWindowEx( 0, L"CWinPlotter", L"CWinPlotter", WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 200, 200, parent, 0, hInstance, this );
	RECT rect;
	GetClientRect( handle, &rect );
	return handle;
}

void CWinPlotter::show(int cmdShow)
{
	::ShowWindow( handle, cmdShow );
}

void CWinPlotter::OnDestroy()
{
	::PostQuitMessage( 0 );
}

void CWinPlotter::OnSize()
{
	UpdateScreenSize();
}

void CWinPlotter::UpdateScreenSize()
{
	RECT rect;
	GetClientRect(handle, &rect);
	engine.SetWindowSize(rect.right - rect.left, rect.bottom - rect.top);
}

void CWinPlotter::OnCreate()
{
	// Создаём тестовый объект - пирамидку
	testObject.AddPoint( C3DPoint( 5, 5, 0 ) );
	testObject.AddPoint( C3DPoint( 5, -5, 0 ) );
	testObject.AddPoint( C3DPoint( -5, -5, 0 ) );
	testObject.AddPoint( C3DPoint( -5, 5, 0 ) );
	testObject.AddPoint( C3DPoint( 0, 0, 5 ) );
	testObject.AddTriangle( 0, 1, 4 );
	testObject.AddTriangle( 1, 2, 4 );
	testObject.AddTriangle( 2, 3, 4 );
	testObject.AddTriangle( 3, 0, 4 );

	// Создаём оси
	axisObject.AddPoint( C3DPoint(axisLength, 0, 0) );
	axisObject.AddPoint( C3DPoint(-axisLength, 0, 0) );
	axisObject.AddSegment( 0, 1 );

	axisObject.AddPoint( C3DPoint(0, axisLength, 0) );
	axisObject.AddPoint( C3DPoint(0, -axisLength, 0) );
	axisObject.AddSegment( 2, 3 );

	axisObject.AddPoint( C3DPoint(0, 0, axisLength) );
	axisObject.AddPoint( C3DPoint(0, 0, -axisLength) );
	axisObject.AddSegment( 4, 5 );

	// Устанавливаем позицию камеры
	engine.SetPosition( C3DPoint( 12, 14, 6 ) );
	engine.SetViewPoint( C3DPoint( 0, 0, 0 ) );
}

void CWinPlotter::PaintObject()
{
	// Передаём размеры окна
	UpdateScreenSize();

	// Делаем рендер объекта
	engine.Render( testObject, renderedObject );
	// Делаем рендер осей
	engine.Render( axisObject, axisRenderedObject );

	RECT rect;
	GetClientRect( handle, &rect );
	PAINTSTRUCT paintStruct;

	HDC currentDC = BeginPaint( handle, &paintStruct );

	HBITMAP currentBitmap;

	currentBitmap = CreateCompatibleBitmap( currentDC, rect.right - rect.left, rect.bottom - rect.top );
	PatBlt( currentDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, BLACKNESS );


	HPEN linePen = ::CreatePen( PS_SOLID, 1, RGB( 0, 255, 0 ) );
	HPEN currentPen = ( HPEN )::SelectObject( currentDC, linePen );

	HBRUSH pointBrush = ::CreateSolidBrush( RGB( 0, 255, 0 ) );
	HBRUSH currentBrush = ( HBRUSH )::SelectObject( currentDC, pointBrush );
	// Вершины
	for (auto point = renderedObject.Points.begin(); point != renderedObject.Points.end(); point++) {
		Ellipse( currentDC, point->X - 1, point->Y - 1, point->X + 1, point->Y + 1);
	}

	// Отрезки
	for( auto segment = renderedObject.Segments.begin(); segment != renderedObject.Segments.end(); segment++ ) {
		MoveToEx( currentDC, renderedObject.Points[segment->First].X, renderedObject.Points[segment->First].Y, 0 );
		LineTo( currentDC, renderedObject.Points[segment->Second].X, renderedObject.Points[segment->Second].Y );
	}
	// Треугольники
	for( auto triangle = renderedObject.Triangles.begin(); triangle != renderedObject.Triangles.end(); triangle++ ) {
		MoveToEx( currentDC, renderedObject.Points[triangle->First].X, renderedObject.Points[triangle->First].Y, 0 );
		LineTo( currentDC, renderedObject.Points[triangle->Second].X, renderedObject.Points[triangle->Second].Y );
		LineTo( currentDC, renderedObject.Points[triangle->Third].X, renderedObject.Points[triangle->Third].Y );
		LineTo( currentDC, renderedObject.Points[triangle->First].X, renderedObject.Points[triangle->First].Y );
	}

	// Кисть для осей координат
	HPEN axisPen = ::CreatePen(PS_SOLID, 1, RGB(41, 128, 185));
	currentPen = (HPEN)::SelectObject(currentDC, axisPen);
	for (auto segment = axisRenderedObject.Segments.begin(); segment != axisRenderedObject.Segments.end(); segment++) {
		MoveToEx(currentDC, axisRenderedObject.Points[segment->First].X, axisRenderedObject.Points[segment->First].Y, 0);
		LineTo(currentDC, axisRenderedObject.Points[segment->Second].X, axisRenderedObject.Points[segment->Second].Y);
	}

	::DeleteObject( pointBrush );
	::DeleteObject( currentBrush );

	::DeleteObject( linePen );
	::DeleteObject( axisPen );

	::SelectObject( currentDC, currentPen );
	::DeleteObject( currentBitmap );
	DeleteDC( currentDC );

	EndPaint( handle, &paintStruct );
}

void CWinPlotter::Invalidate()
{
	RECT rect;
	GetClientRect( handle, &rect );
	InvalidateRect( handle, &rect, true );
}

void CWinPlotter::moveX( LONG times )
{
	engine.MoveSide( times * engineMovementFactor );
	Invalidate();
}

void CWinPlotter::moveY( LONG times )
{
	engine.MoveUp( -times * engineMovementFactor );
	Invalidate();
}

void CWinPlotter::rotateX( LONG times )
{
	engine.RotateSide(times * engineRotationFactor);
	Invalidate();
}

void CWinPlotter::rotateY( LONG times )
{
	engine.RotateUp(times * engineRotationFactor);
	Invalidate();
}

void CWinPlotter::zoom( LONG times )
{
	engine.MoveForward( times * engineZoomFactor );
	Invalidate();
}

void CWinPlotter::clear()
{
	testObject.Clear();
	Invalidate();
}