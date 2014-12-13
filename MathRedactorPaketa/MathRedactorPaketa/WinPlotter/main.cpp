#include <windows.h>
#include "CWinMain.h"
#include "CWinPlotter.h"
#include "MainWindow.h"
#include "resource.h"

int __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE previnstance, LPWSTR commandLine, int nCmdShow ) {
	::CoInitialize( 0 );
	CWinMain::registerClass( hInstance );
	CWinPlotter::registerClass( hInstance );
	CMainWindow::RegisterClass( hInstance );
	CWinMain window;

	HWND hWnd = window.create( hInstance );

	if( hWnd == 0 ) {
		MessageBox( 0, L"Не удалось создать окно", L"Ошибка", MB_OK | MB_ICONERROR );
		return EXIT_FAILURE;
	}
	window.show( nCmdShow );

	MSG msg;
    HACCEL hAccel = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDR_ACCELERATOR1 ) );

    while( ::GetMessage( &msg, 0, 0, 0 ) ) {
        if( !( hAccel && TranslateAccelerator( hWnd, hAccel, &msg ) ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
	}

	return EXIT_SUCCESS;
}
