﻿// Автор: Федюнин Валерий.

#include "CommandHandler.h"
#include "FractionSymbol.h"
#include "SimpleSymbol.h"
#include "SigmaSymbol.h"
#include "IndexSymbol.h"
#include "RootSymbol.h"
#include "RibbonIDs.h"
#include "FunctionSymbol.h"

#include <assert.h>

CCommandHandler::CCommandHandler( CEditWindow* _editWindow ) {
	editWindow = _editWindow;
	referenceCount = 1;
}

// Реализация IUnknown.

// Добавить ссылку.
STDMETHODIMP_( ULONG ) CCommandHandler::AddRef()
{
	return ::InterlockedIncrement( &referenceCount );
}

// Убрать ссылку.
STDMETHODIMP_( ULONG ) CCommandHandler::Release()
{
	return ::InterlockedDecrement( &referenceCount );
}

// Передать объект как указатель на нужный тип.
STDMETHODIMP CCommandHandler::QueryInterface( REFIID type, void** res )
{
	if( type == __uuidof( IUnknown ) ) {
		*res = static_cast< IUnknown* >( this );
	} else if( type == __uuidof( IUICommandHandler ) ) {
		*res = static_cast< IUICommandHandler* >( this );
	} else {
		*res = 0;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

// Реализация IUICommandHandler.

// Изменяет свойства Ribbon'а.
STDMETHODIMP CCommandHandler::UpdateProperty( UINT nCmdId, REFPROPERTYKEY key, const PROPVARIANT* propvarCurrentValue,
	PROPVARIANT* propvarNewValue )
{
	return E_NOTIMPL;
}

// Выполняет нужную команду из этого обработчика.
STDMETHODIMP CCommandHandler::Execute( UINT nCmdId, UI_EXECUTIONVERB verb, const PROPERTYKEY* key,
	const PROPVARIANT* propvarValue, IUISimplePropertySet* pCommandExecutionProperties )
{
	switch( nCmdId ) {
		case ID_CMD_ACCEPT:
			editWindow->SendAccept();
			break;
		case ID_CMD_REJECT:
			editWindow->SendClose();
			break;
		case ID_CMD_EXPORT:
			editWindow->ExportSelected();
			break;
		case ID_CMD_IMPORT:
            editWindow->ImportSelected();
			//::MessageBox( 0, L"Здесь функция импорта", L"TODO", MB_OK );
			break;
		case ID_CMD_CHECK:
			editWindow->CheckValidity();
			break;
		case ID_CMD_SIGMA:
		{
			CSigmaSymbol* sigma = new CSigmaSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), bigSigmaSymbol );
			editWindow->AddSymbol( sigma );
			break;
		}
		case ID_CMD_PIMUL:
		{
			CSigmaSymbol* sigma = new CSigmaSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), bigPiSymbol );
			editWindow->AddSymbol( sigma );
			break;
		}
		case ID_CMD_FRACTION:
		{
			CFractionSymbol* fraction = new CFractionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight() );
			editWindow->AddSymbol( fraction );
			break;
		}
		case ID_CMD_LOWER_INDEX:
		{
			CIndexSymbol* lowerIndex = new CIndexSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight( ), CIndexSymbol::ITLower );
			editWindow->AddSymbol( lowerIndex );
			break;
		}
		case ID_CMD_UPPER_INDEX:
		{
			CIndexSymbol* upperIndex = new CIndexSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), CIndexSymbol::ITUpper );
			editWindow->AddSymbol( upperIndex );
			break;
		}
		case ID_CMD_SQRT:
		{
			CRootSymbol* root = new CRootSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight() );
			editWindow->AddSymbol( root );
			break;
		}
		case ID_CMD_SIN:
		{
			CFunctionSymbol *func = new CFunctionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), L"sin" );
			editWindow->AddSymbol( func );
			break;
		}
		case ID_CMD_COS:
		{
			CFunctionSymbol *func = new CFunctionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), L"cos" );
			editWindow->AddSymbol( func );
			break;
		}
		case ID_CMD_TAN:
		{
			CFunctionSymbol *func = new CFunctionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), L"tan" );
			editWindow->AddSymbol( func );
			break;
		}
		case ID_CMD_CTG:
		{
			CFunctionSymbol *func = new CFunctionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), L"ctg" );
			editWindow->AddSymbol( func );
			break;
		}
		case ID_CMD_Y_FX:
			editWindow->SetFunctionType( FT_YfromX );
			break;
		case ID_CMD_Z_FXY:
			editWindow->SetFunctionType( FT_ZfromXY );
			break;
		case ID_CMD_XY_T:
			editWindow->SetFunctionType( FT_XYfromT );
			break;
		case ID_CMD_XYZ_T:
			editWindow->SetFunctionType( FT_XYZfromT );
			break;
		case ID_CMD_XYZ_TL:
			editWindow->SetFunctionType( FT_XYZfromTL );
			break;
		default:
			assert( false );
	}

	return S_OK;
}