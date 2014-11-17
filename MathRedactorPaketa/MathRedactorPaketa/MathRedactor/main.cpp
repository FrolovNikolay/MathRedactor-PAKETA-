#include <Windows.h>

#include "MainWindow.h"

WPARAM MessageLoop()
{
	MSG message;
	while( ::GetMessage( &message, 0, 0, 0 ) != 0 ) {
		::TranslateMessage( &message );
		::DispatchMessage( &message );
	}

	return message.wParam;
}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR commandLine, int nCmdShow)
{
	::CoInitialize( 0 );
	CMainWindow::RegisterClass( hInstance );

	CMainWindow mainWindow;
	mainWindow.Create( L"Редактор формул", hInstance );
	mainWindow.Show( nCmdShow );

	WPARAM exitCode = MessageLoop();

	return exitCode;
}