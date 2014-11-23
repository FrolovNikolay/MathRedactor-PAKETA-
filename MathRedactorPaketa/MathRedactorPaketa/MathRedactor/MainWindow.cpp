#include "MainWindow.h"

// класс CMainWindow
// константы

const wchar_t* CMainWindow::className = L"MathRedactorWindowClass";

// public методы 

CMainWindow::CMainWindow() 
{
	referenceCount = 1;
	windowHandle = 0;
	editWindow = new CEditWindow();
	commandHandler = new CCommandHandler( editWindow );
	uiFramework = 0;
}

CMainWindow::~CMainWindow()
{
	if( editWindow != 0 ) {
		delete( editWindow );
	}
}

bool CMainWindow::RegisterClass( HINSTANCE classOwnerInstance )
{
	WNDCLASSEX classInfo;
	::ZeroMemory( &classInfo, sizeof( WNDCLASSEX ) );
	classInfo.cbSize = sizeof( WNDCLASSEX );
	classInfo.hInstance = classOwnerInstance;
	classInfo.lpszClassName = className;
	classInfo.style = CS_HREDRAW | CS_VREDRAW;
	classInfo.lpfnWndProc = &CMainWindow::windowProcedure;

	return ( ::RegisterClassEx( &classInfo ) != 0 );
}

HWND CMainWindow::Create( HINSTANCE ownerInstance, HWND parent )
{	
	DWORD style = WS_OVERLAPPED | WS_CLIPCHILDREN;
	parentHandle = parent;
	windowHandle = ::CreateWindowEx( 0, className, L"Редактор формул", style, 0, 0, 800, 600, parent, 0, ownerInstance, this );
	return windowHandle;
}

void CMainWindow::Show( int nCmdShow ) 
{
	::ShowWindow( windowHandle, nCmdShow );
}

STDMETHODIMP_( ULONG ) CMainWindow::AddRef()
{
	return ::InterlockedIncrement( &referenceCount );
}

STDMETHODIMP_( ULONG ) CMainWindow::Release()
{
	return ::InterlockedDecrement( &referenceCount );
}

STDMETHODIMP CMainWindow::QueryInterface( REFIID type, void** res )
{
	if( type == __uuidof( IUnknown ) ) {
		*res = static_cast< IUnknown* >( this );
	} else if( type == __uuidof( IUIApplication ) ) {
		*res = static_cast< IUIApplication* >( this );
	}
	else {
		*res = 0;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

STDMETHODIMP CMainWindow::OnCreateUICommand( UINT nCmdId, UI_COMMANDTYPE typeId, IUICommandHandler** _commandHandler )
{
	return commandHandler->QueryInterface( IID_PPV_ARGS( _commandHandler ) );
}

STDMETHODIMP CMainWindow::OnViewChanged( UINT viewId, UI_VIEWTYPE typeId, IUnknown* view, UI_VIEWVERB verb, INT reason )
{
	HRESULT res = E_NOTIMPL;

	IUIRibbon* ribbon = 0;
	if( typeId == UI_VIEWTYPE_RIBBON ) {
		switch( verb )
		{
		case UI_VIEWVERB_CREATE:
			res = S_OK;
			break;
		case UI_VIEWVERB_SIZE:
			res = view->QueryInterface( IID_PPV_ARGS( &ribbon ) );
			if( SUCCEEDED( res ) ) {
				res = ribbon->GetHeight( &ribbonHeight );
				ribbon->Release();
			}
			break;
		case UI_VIEWVERB_DESTROY:
			res = S_OK;
			break;
		}
	}

	return res;
}

STDMETHODIMP CMainWindow::OnDestroyUICommand( UINT32 nCmdId, UI_COMMANDTYPE typeId, IUICommandHandler* _commandHandler ) 
{
	return E_NOTIMPL;
}

// protected методы

void CMainWindow::OnWmDestroy()
{
	destroyFramework();
	::PostQuitMessage( 0 );
}

void CMainWindow::OnWmClose()
{
	Show( SW_HIDE );
	::EnableWindow( parentHandle, true );
	::SetFocus( parentHandle );
}

void CMainWindow::OnWmCreate( HWND _windowHandle )
{

	windowHandle = _windowHandle;
	bool reg = CEditWindow::RegisterClass( ::GetModuleHandle( 0 ) );

	editHandle = editWindow->Create( windowHandle, ::GetModuleHandle( 0 ) );
	initializeUIFramework();
}

void CMainWindow::OnWmSize()
{
	RECT clientRect;
	::GetClientRect( windowHandle, &clientRect );

	::SetWindowPos( editHandle, 0, 0, ribbonHeight, clientRect.right, clientRect.bottom - ribbonHeight, 0 );
}

void CMainWindow::OnWmChar( WPARAM code )
{
	editWindow->AddSign( code );
	if( code == 0x0D ) {
		editWindow->NewLine();
	}
}

void CMainWindow::OnWmKeydown( WPARAM code )
{
	switch( code ) {
	case VK_BACK:
		editWindow->RemoveSign();
		break;
	case VK_LEFT:
		editWindow->MoveCaret( CEditWindow::CD_Left );
		break;
	case VK_RIGHT:
		editWindow->MoveCaret( CEditWindow::CD_Right );
		break;
	case VK_UP:
		editWindow->MoveCaret( CEditWindow::CD_Up );
		break;
	case VK_DOWN:
		editWindow->MoveCaret( CEditWindow::CD_Down );
		break;
	}
}

void CMainWindow::OnWmSetFocus()
{
	editWindow->ShowCaret();
}

void CMainWindow::OnWmKillFocus()
{
	editWindow->HideCaret();
}

// private методы
// инициализирует фремворк для работы риббона
void CMainWindow::initializeUIFramework()
{
	::CoCreateInstance( CLSID_UIRibbonFramework, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &uiFramework ) );
	uiFramework->Initialize( windowHandle, this );
	uiFramework->LoadUI( ::GetModuleHandle( 0 ), L"APPLICATION_RIBBON" );
}

// уничтожает ui фреймворк
void CMainWindow::destroyFramework()
{
	uiFramework->Destroy();
	uiFramework->Release();
	uiFramework = 0;
}

// процедура обрабатывающая сообщения для главного окна
LRESULT __stdcall CMainWindow::windowProcedure( HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_NCCREATE ) {
		::SetWindowLong( windowHandle, GWL_USERDATA, reinterpret_cast<LONG>( reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams ) );
	}

	CMainWindow* window = reinterpret_cast<CMainWindow*>( ::GetWindowLong( windowHandle, GWL_USERDATA ) );	

	switch( message ) {
	case WM_REDACTOR_OK:
		::SendMessage( window->parentHandle, WM_REDACTOR_OK, 0, 0 );
		window->OnWmClose();
		return 0;
	case WM_DESTROY:
		window->OnWmDestroy();
		break;
	case WM_CLOSE:
		window->OnWmClose();
		return 0;
	case WM_CREATE:
		window->OnWmCreate( windowHandle );
		break;
	case WM_SIZE:
		window->OnWmSize();
		break;
	case WM_CHAR:
		window->OnWmChar( wParam );
		break;
	case WM_KEYDOWN:
		window->OnWmKeydown( wParam );
		break;
	case WM_SETFOCUS:
		window->OnWmSetFocus();
		break;
	case WM_KILLFOCUS:
		window->OnWmKillFocus();
		break;
	}

	return ::DefWindowProc( windowHandle, message, wParam, lParam );
}