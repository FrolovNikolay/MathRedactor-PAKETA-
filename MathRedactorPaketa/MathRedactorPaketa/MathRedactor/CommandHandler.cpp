// Автор: Федюнин Валерий

#include "FractionSymbol.h"
#include "SimpleSymbol.h"
#include "SigmaSymbol.h"
#include "IndexSymbol.h"
#include "RootSymbol.h"
#include "CommandHandler.h"
#include "RibbonIDs.h"
#include "FunctionSymbol.h"

CCommandHandler::CCommandHandler( CEditWindow* _editWindow) {
	editWindow = _editWindow;
	referenceCount = 1;
}

STDMETHODIMP_( ULONG ) CCommandHandler::AddRef()
{
	return ::InterlockedIncrement( &referenceCount );
}

STDMETHODIMP_( ULONG ) CCommandHandler::Release()
{
	return ::InterlockedDecrement( &referenceCount );
}

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

STDMETHODIMP CCommandHandler::UpdateProperty( UINT nCmdId, REFPROPERTYKEY key, const PROPVARIANT* propvarCurrentValue,
	PROPVARIANT* propvarNewValue )
{
	return E_NOTIMPL;
}

STDMETHODIMP CCommandHandler::Execute( UINT nCmdId, UI_EXECUTIONVERB verb, const PROPERTYKEY* key,
	const PROPVARIANT* propvarValue, IUISimplePropertySet* pCommandExecutionProperties )
{
	switch( nCmdId ) {
		case ID_CMD_SIGMA:
		{
			CSigmaSymbol* sigma = new CSigmaSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), bigSigmaSymbol );
			sigma->GetUpperLine().Insert( 0, new CSimpleSymbol( L'9' ) );
			sigma->GetLowerLine().Insert( 0, new CSimpleSymbol( L'1' ) );
			editWindow->AddSymbol( sigma );
			break;
		}
		case ID_CMD_PIMUL:
		{
			CSigmaSymbol* sigma = new CSigmaSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), bigPiSymbol );
			sigma->GetUpperLine().Insert( 0, new CSimpleSymbol( L'9' ) );
			sigma->GetLowerLine().Insert( 0, new CSimpleSymbol( L'1' ) );
			editWindow->AddSymbol( sigma );
			break;
		}
		case ID_CMD_FRACTION:
		{
			CFractionSymbol* fraction = new CFractionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight() );
			fraction->GetUpperLine().Insert( 0, new CSimpleSymbol( L'1' ) );
			fraction->GetLowerLine().Insert( 0, new CSimpleSymbol( L'2' ) );
			editWindow->AddSymbol( fraction );
			break;
		}
		case ID_CMD_LOWER_INDEX:
		{
			CIndexSymbol* lowerIndex = new CIndexSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight( ), CIndexSymbol::ITLower );
			lowerIndex->GetLine().Insert( 0, new CSimpleSymbol( L't' ) );
			lowerIndex->GetLine().Insert( 1, new CSimpleSymbol( L'm' ) );
			lowerIndex->GetLine().Insert( 2, new CSimpleSymbol( L'p' ) );
			editWindow->AddSymbol( lowerIndex );
			break;
		}
		case ID_CMD_UPPER_INDEX:
		{
			CIndexSymbol* upperIndex = new CIndexSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), CIndexSymbol::ITUpper );
			upperIndex->GetLine().Insert( 0, new CSimpleSymbol( L't' ) );
			upperIndex->GetLine().Insert( 1, new CSimpleSymbol( L'm' ) );
			upperIndex->GetLine().Insert( 2, new CSimpleSymbol( L'p' ) );
			editWindow->AddSymbol( upperIndex );
			break;
		}
		case ID_CMD_SQRT:
		{
			CRootSymbol* root = new CRootSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight() );
			root->GetExponentLine().Insert( 0, new CSimpleSymbol( L'2' ) );
			root->GetRadicandLine().Insert( 0, new CSimpleSymbol( L'x' ) );
			editWindow->AddSymbol( root );
			break;
		}
		case ID_CMD_SIN:
		{
			CFunctionSymbol *func = new CFunctionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), L"sin" );
			func->GetArgumentLine().PushBack( new CSimpleSymbol( L'x' ) );
			editWindow->AddSymbol( func );
			break;
		}
		case ID_CMD_COS:
		{
			CFunctionSymbol *func = new CFunctionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), L"cos" );
			func->GetArgumentLine().PushBack( new CSimpleSymbol( L'x' ) );
			editWindow->AddSymbol( func );
			break;
		}
		case ID_CMD_TAN:
		{
			CFunctionSymbol *func = new CFunctionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), L"tan" );
			func->GetArgumentLine().PushBack( new CSimpleSymbol( L'x' ) );
			editWindow->AddSymbol( func );
			break;
		}
		case ID_CMD_CTG:
		{
			CFunctionSymbol *func = new CFunctionSymbol( editWindow->GetCaretLine()->GetSimpleSymbolHeight(), L"ctg" );
			func->GetArgumentLine().PushBack( new CSimpleSymbol( L'x' ) );
			editWindow->AddSymbol( func );
			break;
		}
	}

	return S_OK;
}