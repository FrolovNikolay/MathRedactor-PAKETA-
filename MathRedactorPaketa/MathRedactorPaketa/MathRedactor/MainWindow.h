// Автор: Федюнин Валерий
// Описание: класс главного окна приложения

#pragma once

#include <Windows.h>
#include <memory>
#include <UIRibbon.h>

#include "EditWindow.h"
#include "CommandHandler.h"

// Класса, соответствующий главному окну приложения
class CMainWindow : public IUIApplication {
public:
	CMainWindow();
	~CMainWindow();

	// регистрирует класс окна, 
	static bool RegisterClass( HINSTANCE );
	
	// Создает окно
	HWND Create( HINSTANCE ownerInstance, HWND parent );
	// Отображает окно
	void Show( int );

	// реализация IUnknown
	// добавить ссылку
	STDMETHOD_( ULONG, AddRef() );
	// убрать ссылку
	STDMETHOD_( ULONG, Release() );
	// передать объект как указатель на нужный тип
	STDMETHOD( QueryInterface( REFIID type, void** res ) );

	// реализация IUIApplication
	// связывает команду с хэндлером, обрабатывающим ее
	STDMETHOD( OnCreateUICommand )( UINT nCmdId, UI_COMMANDTYPE typeId, IUICommandHandler** _commandHandler );
	// вызывается для каждой команды при уничтожении окна
	STDMETHOD( OnDestroyUICommand )( UINT32 nCmdId, UI_COMMANDTYPE typeId, IUICommandHandler* _commandHandler );
	// вызывается при изменении состояния Ribbon'а
	STDMETHOD( OnViewChanged )( UINT viewId, UI_VIEWTYPE typeId, IUnknown* view, UI_VIEWVERB verb, INT reasonCode );

protected:
	// метод, вызываемый при получении окном сообщения WM_DESTROY
	void OnWmDestroy();
	// метод, вызываемый при получении окном сообщения WM_CLOSE
	void OnWmClose();
	// метод, создающий дочерние окна, вызывается при сообщени WM_CREATE
	void OnWmCreate( HWND );
	// меняет размеры дочерних окон при изменении размеров родительского
	void OnWmSize();
	// заставляет дочернее окно отображать новый символ
	void OnWmChar( WPARAM );
	// обрабатывает сообщения с системных клавиш
	void OnWmKeydown( WPARAM );
	// обрабатывает смену фокуса (скрывает/показывает каретку)
	void OnWmSetFocus();
	void OnWmKillFocus();

private:
	// хэндл окна, которому соответствует этот объект класса.
	HWND windowHandle;
	// дочернее окно, в котором отображабтся и редактируются формулы
	CEditWindow *editWindow;
	// хэндл дочернего окна-редактора
	HWND editHandle;
	// хендл родителя
	HWND parentHandle;
	// имя класса окна
	static const wchar_t* className;

	// указатель на UI framework
	IUIFramework* uiFramework;
	// количество ссылок на объект (требуется для Ribbon)
	ULONG referenceCount;
	// обработчик команд Ribbon'а
	CCommandHandler* commandHandler;
	// высота Ribbon'а
	UINT32 ribbonHeight;

	void initializeUIFramework();
	void destroyFramework();

	static LRESULT __stdcall windowProcedure( HWND, UINT, WPARAM, LPARAM );
};