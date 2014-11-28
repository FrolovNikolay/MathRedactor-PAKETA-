// Автор: Федюнин Валерий.

// Описание: обработчик команд Ribbon'а.

#pragma once

#include <UIRibbon.h>
#include "EditWindow.h"

// обработчик команд Ribbon'а в приложении MathRedactor.
class CCommandHandler : public IUICommandHandler {
public:
	CCommandHandler( CEditWindow* );

	// Реализация IUnknown.

	// Добавить ссылку.
	STDMETHOD_( ULONG, AddRef() );
	// Убрать ссылку.
	STDMETHOD_( ULONG, Release() );
	// Передать объект как указатель на нужный тип.
	STDMETHOD( QueryInterface( REFIID type, void** res ) );

	// Реализация IUICommandHandler.

	// Изменяет свойства Ribbon'а.
	STDMETHOD( UpdateProperty )( UINT nCmdID, REFPROPERTYKEY key, 
		const PROPVARIANT* ppropvarCurrentValue, PROPVARIANT* ppropvarNewValue );
	// Выполняет нужную команду из этого обработчика.
	STDMETHOD( Execute )( UINT nCmdID, UI_EXECUTIONVERB verb, const PROPERTYKEY* key,
		const PROPVARIANT* ppropvarValue, IUISimplePropertySet* pCommandExecutionProperties );

private:
	// количество ссылок на этот обработчик
	ULONG referenceCount;
	// окно редактор
	CEditWindow* editWindow;
};