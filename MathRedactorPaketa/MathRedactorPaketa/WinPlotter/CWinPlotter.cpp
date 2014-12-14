#include "CWinPlotter.h"
#include <Windows.h>
#include "resource.h"
#include <fstream>

#include "3DPoint.h"
#pragma comment (lib, "Msimg32.lib")


namespace Color
{
	COLOR16 GetRed( double z, double max )
	{
		if( 2 * z > max ) {
			return (2 * z / max - 1) * 0x00ff00;
		} else {
			return 0;
		}
	}

	COLOR16 GetGreen( double z, double max )
	{
		if( 2 * z > max ) {
			return ( 2 - 2 * z / max ) * 0x00ff00;
		} else {
			return ( 2 * z / max ) * 0x00ff00;
		}
	}

	COLOR16 GetBlue( double z, double max )
	{
		if( 2 * z < max ) {
			return ( 1 - 2 * z / max ) * 0x00ff00;
		} else {
			return 0;
		}
	}
}

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
	windowClass.hCursor = ::LoadCursor( hInstance, IDC_CROSS );

	ATOM res = ::RegisterClassEx( &windowClass );
	return ( res != 0 );
}

HWND CWinPlotter::create( HINSTANCE hInstance, HWND parent )
{
	DWORD style = WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS;
	parentHandle = parent;
	handle = ::CreateWindowEx( 0, L"CWinPlotter", L"CWinPlotter", style, 0, 0, 200, 200, parent, 0, hInstance, this );
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
	engine.SetWindowSize( rect.right - rect.left, rect.bottom - rect.top );
}

void CWinPlotter::OnCreate()
{
	// Создаём тестовый объект - РАКЕТА
	std::ifstream testText( "PAKETA.txt", std::ifstream::in );
	int points, segments;
	const double coef = 2;
	testText >> points >> segments;
	for( int i = 0; i < points; i++ ) {
		double x, z;
		testText >> x >> z;
		testObject.AddPoint( C3DPoint( ( x - 4.25 ) * coef, 0, ( z + 0.1 ) * coef ) );
	}
	for( int i = 0; i < segments; i++ ) {
		int first, second;
		testText >> first >> second;
		testObject.AddSegment( first, second );
	}

	// Создаём оси
	axisObject.AddPoint( C3DPoint( axisLength, 0, 0 ) );
	axisObject.AddPoint( C3DPoint( -axisLength, 0, 0 ) );
	axisObject.AddSegment( 0, 1 );

	axisObject.AddPoint( C3DPoint( 0, axisLength, 0 ) );
	axisObject.AddPoint( C3DPoint( 0, -axisLength, 0 ) );
	axisObject.AddSegment( 2, 3 );

	axisObject.AddPoint( C3DPoint( 0, 0, axisLength ) );
	axisObject.AddPoint( C3DPoint( 0, 0, -axisLength ) );
	axisObject.AddSegment( 4, 5 );
}

void CWinPlotter::PaintObject()
{
	// Передаём размеры окна
	UpdateScreenSize();

	// Делаем рендер объекта
	engine.Render( testObject, renderedObject );
	// Делаем рендер осей
	engine.Render( axisObject, axisRenderedObject, false );

	RECT rect;
	GetClientRect( handle, &rect );
	PAINTSTRUCT paintStruct;

	HDC currentDC = BeginPaint( handle, &paintStruct );
	PatBlt( currentDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, BLACKNESS );
	
	HPEN linePen = ::CreatePen( PS_SOLID, 1, RGB( 0, 255, 0 ) );
	HPEN currentPen = ( HPEN )::SelectObject( currentDC, linePen );

	HBRUSH pointBrush = ::CreateSolidBrush( RGB( 0, 255, 0 ) );
	HBRUSH currentBrush = ( HBRUSH )::SelectObject( currentDC, pointBrush );
	// Вершины
	/* может потом и потребуется их рисовать, но сейчас они лишь мешают
	for( auto point = renderedObject.Points.begin(); point != renderedObject.Points.end(); point++ ) {
		Ellipse( currentDC, static_cast< int >( point->X - 1 ), static_cast< int >( point->Y - 1 ),
			static_cast< int >( point->X + 1 ), static_cast< int >( point->Y + 1 ) );
	}
	*/

	// Отрезки
	for( auto segment = renderedObject.Segments.begin(); segment != renderedObject.Segments.end(); segment++ ) {
		MoveToEx( currentDC,
			static_cast< int >( renderedObject.Points[segment->First].X ),
			static_cast< int >( renderedObject.Points[segment->First].Y ),
			0 );
		LineTo( currentDC,
			static_cast< int >( renderedObject.Points[segment->Second].X ),
			static_cast< int >( renderedObject.Points[segment->Second].Y ) );
	}

	// Треугольники
	TRIVERTEX* vertex = new TRIVERTEX[renderedObject.Points.size()];
	GRADIENT_TRIANGLE* gTriangle = new GRADIENT_TRIANGLE[renderedObject.Triangles.size()];

	for( int i = 0; i < renderedObject.Points.size(); i++ ) {
		vertex[i].x = renderedObject.Points[i].X;
		vertex[i].y = renderedObject.Points[i].Y;
		vertex[i].Red = Color::GetRed( testObject.Points[i].Z - minZ, maxZ - minZ );
		vertex[i].Green = Color::GetGreen( testObject.Points[i].Z - minZ, maxZ - minZ );
		vertex[i].Blue = Color::GetBlue( testObject.Points[i].Z - minZ, maxZ - minZ );
		vertex[i].Alpha = 0x0000;
	}

	for( int i = 0; i < renderedObject.Triangles.size(); i++ ) {
		gTriangle[i].Vertex1 = renderedObject.Triangles[i].First;
		gTriangle[i].Vertex2 = renderedObject.Triangles[i].Second;
		gTriangle[i].Vertex3 = renderedObject.Triangles[i].Third;	
	}

	GradientFill( currentDC, vertex, renderedObject.Points.size(), gTriangle, renderedObject.Triangles.size(), GRADIENT_FILL_TRIANGLE );

	// Кисть для осей координат
	HPEN axisPen = ::CreatePen( PS_SOLID, 1, RGB( 41, 128, 185 ) );
	currentPen = ( HPEN )::SelectObject( currentDC, axisPen );
	for( auto segment = axisRenderedObject.Segments.begin(); segment != axisRenderedObject.Segments.end(); segment++ ) {
		MoveToEx( currentDC,
			static_cast< int >( axisRenderedObject.Points[segment->First].X ),
			static_cast< int >( axisRenderedObject.Points[segment->First].Y ),
			0 );
		LineTo( currentDC,
			static_cast< int >( axisRenderedObject.Points[segment->Second].X ),
			static_cast< int >( axisRenderedObject.Points[segment->Second].Y ) );
	}

	::DeleteObject( pointBrush );
	::DeleteObject( currentBrush );

	::DeleteObject( linePen );
	::DeleteObject( axisPen );

	::SelectObject( currentDC, currentPen );
	DeleteDC( currentDC );

	EndPaint( handle, &paintStruct );
}

void CWinPlotter::Invalidate()
{
	RECT rect;
	GetClientRect( handle, &rect );
	InvalidateRect( handle, &rect, true );
}

void CWinPlotter::rotateX( LONG times )
{
	engine.RotateSideAroundCenter( -times * engineRotationFactor );
	Invalidate();
}

void CWinPlotter::rotateY( LONG times )
{
	engine.RotateUpAroundCenter( -times * engineRotationFactor );
	Invalidate();
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

void CWinPlotter::zoom( LONG times )
{
	engine.MoveForward( times * engineZoomFactor );
	Invalidate();
}

void CWinPlotter::reset()
{
	engine.Reset();
	Invalidate();
}