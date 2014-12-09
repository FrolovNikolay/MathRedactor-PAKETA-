// Автор: Федюнин Валерий.

#include "EditWindow.h"
#include "SimpleSymbol.h"
#include <MathValidator.h>
#include <TranslatorDLLInterface.h>

#include <assert.h>
#include <windowsx.h>

// Имя класса окна.
const wchar_t* CEditWindow::className = L"MathRedactorEditWindowClass";

// public методы.

CEditWindow::CEditWindow() 
		: horizontalScrollUnit( 30 )
		, verticalScrollUnit( 15 )
		, simpleSymbolHeight( 50 )
		, content( 1, CLineOfSymbols( simpleSymbolHeight ) )
		, caret( this, &content[0] )
		, finder( content )
		, symbolSelector( finder, content )
		, drawer( content, symbolSelector, horizontalScrollUnit, verticalScrollUnit, simpleSymbolHeight )
		, currentFunctionType( FT_Undefined )
{
	SetFunctionType( FT_YfromX );
	windowHandle = 0;
}

// Регистрирует класс окна.
bool CEditWindow::RegisterClass( HINSTANCE classOwnerInstance )
{
	WNDCLASSEX classInfo;
	::ZeroMemory( &classInfo, sizeof( WNDCLASSEX ) );
	classInfo.cbSize = sizeof( WNDCLASSEX );
	classInfo.hInstance = classOwnerInstance;
	classInfo.lpszClassName = className;
	classInfo.style = CS_HREDRAW | CS_VREDRAW;
	classInfo.lpfnWndProc = &CEditWindow::windowProcedure;
	classInfo.hCursor = ::LoadCursor( classOwnerInstance, IDC_IBEAM );

	return ( ::RegisterClassEx( &classInfo ) != 0 );
}

// Создать окно.
HWND CEditWindow::Create( HWND parent, HINSTANCE ownerInstance )
{
	DWORD style = WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
	parentHandle = parent;
	windowHandle = ::CreateWindowEx( 0, className, 0, style, 0, 0, 0, 0, parent, 0, ownerInstance, this );

	return windowHandle;
}

// Отобразить окно.
void CEditWindow::Show( int nCmdShow )
{
	::ShowWindow( windowHandle, nCmdShow );
}

// Отображает каретку.
void CEditWindow::ShowCaret()
{
	if( !caret.IsShown() ) {
		caret.Create();
		caret.Show();
	}
}

// Скрывает каретку.
void CEditWindow::HideCaret()
{
	if( caret.IsShown() ) {
		caret.Hide();
		caret.Destroy();
	}
}

// Двигает каретку на шаг по направлению.
void CEditWindow::MoveCaret( CEditWindow::TCaretDirection direction )
{
	// Если были выделенные символы, то перемещаем особым образом.
	if( symbolSelector.HasSelection() ) {
		if( direction == CD_Left || direction == CD_Up ) {
			caret.MoveTo( symbolSelector.GetSelectionInfo().first );
		} else {
			caret.MoveTo( symbolSelector.GetSelectionInfo().second );
			caret.Move( CD_Right );
		}
		symbolSelector.ResetSelection();
		ShowCaret();
	} else {
		caret.Move( direction );
	}
	ShowCaret();
	::InvalidateRect( windowHandle, 0, true );
}

// Добавляет символ и уведомляет окно о том, что надо перерисоваться.
void CEditWindow::AddSymbol( CSymbol* symbol )
{
	// Если были выделенны символы, то удаляем их и отображаем каретку.
	if( symbolSelector.HasSelection() ) {
		removeSelectedItems();
		symbolSelector.ResetSelection();
		ShowCaret();
	}
	
	// Если добавляется сложный символ, необходимо установить родителя его подстрок.
	std::vector<CLineOfSymbols*> substrings;
	symbol->GetSubstrings( substrings );
	CLineOfSymbols* currentLine = GetCaretLine();
	for( size_t i = 0; i < substrings.size(); ++i ) {
		substrings[i]->SetParent( currentLine );
	}

	// После вставки перемещает каретку и пересчитываем связные параметры.
	currentLine->Insert( caret.GetIndex(), symbol );
	caret.Move( CD_Right );
	recalculateHorzScrollParams();
	recalculateVertScrollParams();
	::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE | RDW_ERASE );
}

// Добавляет знак с клавиатуры.
void CEditWindow::AddSign( wchar_t sign )
{
	// Если символ не разрешен, то не делаем ничего.
	if( isSymbolAllowed( sign ) ) {
		AddSymbol( new CSimpleSymbol( sign ) );
		ShowCaret();
		::InvalidateRect( windowHandle, 0, true );
	}
}

// Удаляет символ по текущему положению каретки.
void CEditWindow::RemoveSign()
{
	// Если были выделены символы, то просто удаляем их.
	if( symbolSelector.HasSelection() ) {
		removeSelectedItems();
		symbolSelector.ResetSelection();
		ShowCaret();
	} else {
		// Иначе избавляемся от одного символа и пересчитываем параметры.
		CLineOfSymbols* currentLine = GetCaretLine();
		if( caret.GetIndex() > firstEnablePosition || ( getBaseLineIndex( caret.GetLine() ) == -1 && caret.GetIndex() > 0 ) ) {
			currentLine->Delete( caret.GetIndex() - 1 );
			caret.Move( CD_Left );
		}
		currentLine->Recalculate();
		recalculateHorzScrollParams();
		::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
	}
}

// Переводит строчку.
void CEditWindow::NewLine()
{
	// Если были выделены символы, то удаляем их.
	if( symbolSelector.HasSelection() ) {
		removeSelectedItems();
		symbolSelector.ResetSelection();
		ShowCaret();
	}

	// Добавляем строку в зависимости от текущего положения каретки.
	CLineOfSymbols* currentLine = GetCaretLine();
	int lineIndex = getBaseLineIndex( currentLine );
	if( lineIndex != -1 ) {
		if( lineIndex == content.size() - 1 ) {
			content.push_back( CLineOfSymbols( simpleSymbolHeight, true ) );
		} else {
			content.insert( content.begin() + lineIndex + 1, CLineOfSymbols( simpleSymbolHeight, true ) );
		}
		for( int i = caret.GetIndex(); i < content[lineIndex].Length(); ++i ) {
			content[lineIndex + 1].Insert( i - caret.GetIndex(), content[lineIndex][i]->Clone( &content[lineIndex + 1] ) );
		}
		for( int i = content[lineIndex].Length() - 1; i >= caret.GetIndex(); --i ) {
			content[lineIndex].Delete( i );
		}
		caret.MoveTo( CSymbolPosition( 0, &content[lineIndex + 1] ) );
		recalculateVertScrollParams();
		::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
	}
}

// Получить содержимое редактора в формате для плоттера.
std::string CEditWindow::CalculateStringForPlotter() const
{
	std::string result;
	for( size_t i = 0; i < content.size(); ++i ) {
		// Для каждой из строк добавляем описание функции.
		result += content[i][0]->ToPlotterString();
		result += "=";

		// Далее добавляем саму фнкцию.
		result += content[i].ToPlotterString( firstEnablePosition );
		if( i + 1 != content.size() ) {
			// Разделитель между формулами.
			 result += ", ";
		} else {
			result += "\n";
		}
	}
	return result;
}

// Получить содержимое редактора в Latex формате.
std::string CEditWindow::CalculateLatexString() const
{
	std::string result;
	for( size_t i = 0; i < content.size(); ++i ) {
		result += content[i].ToLatexString() + "\n";
	}
	return result;
}

// Установка нового типа обрабатываемой функции.
void CEditWindow::SetFunctionType( TFunctionType fType )
{
	if( fType == currentFunctionType ) {
		return;
	}

	// Проверяем были ли какой-либо ввод.
	bool hasChanges = false;
	for( size_t i = 0; i < content.size(); ++i ) {
		if( content[i].Length() > firstEnablePosition ) {
			hasChanges = true;
			break;
		}
	}

	// Если что-то было введено, то предупреждаем пользователя о возможной потере информации.
	if( hasChanges ) {
		if( IDNO == ::MessageBox( 0, L"Смена типа вводимой функции повлечет потерю уже введенных данных. Продолжить?",
					  L"Осторожно, возможна потеря данных!", MB_YESNO | MB_ICONWARNING ) ) {
			return;
		}
	}

	// Устанавливаем содержимое окна в соответствии с выбором пользователя.
	symbolSelector.ResetSelection();
	content.clear();
	knownVariables.clear();
	// Важно! Установка типа должна произойти до изменяющих устанавливающих функций.
	currentFunctionType = fType;
	setVariables();
	setFirstEnablePosition();
	setBaseContent();
	caret.MoveTo( CSymbolPosition( firstEnablePosition, &content[0] ) );
	::InvalidateRect( windowHandle, 0, true );
}

// Реакция на кнопку по осуществлению проверки на валидность.
void CEditWindow::CheckValidity() const
{
	std::wstring error;
	if( isInputValid( error ) ) {
		::MessageBox( 0, L"Введенная в окне функция вычислима.", L"Проверка прошла успешно!", MB_OK );
	} else {
		std::wstring msg = std::wstring( L"Введенная в окне функция невычислима. " ) + error;
		::MessageBox( 0, msg.c_str(), L"Введенное выражение невычислимо.", MB_OK );
	}
}

// Экспортировать выделенную часть ввода.
void CEditWindow::ExportSelected() const
{
	if( !symbolSelector.HasSelection() ) {
		::MessageBox( 0, L"Выделите необходимую для экспорта часть функции.", L"Нечего экспортировать!", MB_OK );
	} else {

		OPENFILENAME openFileName;
		WCHAR szFileName[MAX_PATH] = L"";
		::ZeroMemory( &openFileName, sizeof( openFileName ) );
		openFileName.lStructSize = sizeof( OPENFILENAME );
		openFileName.hwndOwner = windowHandle;
		openFileName.lpstrFilter = reinterpret_cast<LPCWSTR>( L"Latex File (*.tex)\0*.tex\0MathMl File (*.mml)\0*.mml\0OpenMath File (*.xml)\0*.xml\0" );
		openFileName.lpstrFile = reinterpret_cast<LPWSTR>( szFileName );
		openFileName.nMaxFile = MAX_PATH;
		openFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		openFileName.lpstrDefExt = reinterpret_cast<LPWSTR>( L"tex" );

		if( ::GetSaveFileName( &openFileName ) ) {
			if( !writeSelectedInFile( openFileName.lpstrFile, openFileName.nFileExtension ) ) {
				::MessageBox( 0, L"При экспорте файла что-то пошло не так. Возможно, что файл сохранен некорректно.",
					  L"Непредвиденная ошибка.", MB_OK | MB_ICONWARNING );
			}
		}
	}
}

// Импортировать формулу из файла.
void CEditWindow::ImportSelected() const 
{
    OPENFILENAME openFileName;
    WCHAR szFileName[MAX_PATH] = L"";
    ::ZeroMemory( &openFileName, sizeof( openFileName ) );
    openFileName.lStructSize = sizeof( OPENFILENAME );
    openFileName.hwndOwner = windowHandle;
    openFileName.lpstrFilter = reinterpret_cast<LPCWSTR>( L"Latex File (*.tex)\0*.tex\0MathMl File (*.mml)\0*.mml\0OpenMath File (*.xml)\0*.xml\0" );
    openFileName.lpstrFile = reinterpret_cast<LPWSTR>( szFileName );
    openFileName.nMaxFile = MAX_PATH;
    openFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    openFileName.lpstrDefExt = reinterpret_cast<LPWSTR>( L"tex" );

    if( ::GetOpenFileName( &openFileName ) ) {
        if( !readSelectedFromFile( openFileName.lpstrFile, openFileName.nFileExtension ) ) {
            ::MessageBox( 0, L"При импорте файла что-то пошло не так. Возможно, что файл некорректен.",
                L"Непредвиденная ошибка.", MB_OK | MB_ICONWARNING );
        }
    }
}


// Передать плоттеру содержимое и сообщить родительскому окну о закрытии.
void CEditWindow::SendAccept() const
{
	std::wstring error;
	// Если введенная формула невалидна, то передача ее плоттеру невозможна.
	if( !isInputValid( error ) ) {
		std::wstring msg = std::wstring( L"Построение графика невозможно. Введенная в окне функция невычислима. " ) + error;
		::MessageBox( 0, msg.c_str(), L"Введенное выражение невычислимо.", MB_OK );
	} else {
		::SendMessage( parentHandle, WM_REDACTOR_OK, 0, 0 );
	}
}

// protected методы.

// Метод, вызываемый при получении окном сообщения WM_DESTROY.
void CEditWindow::OnWmDestroy() {
	::PostQuitMessage( 0 );
}

// Метод, вызываемый при получении окном сообщения WM_PAINT.
void CEditWindow::OnWmPaint()
{
	// мы запоминаем состояние каретки до перерисовки
	bool caretShown = caret.IsShown();
	if( caretShown ) {
		caret.Hide();
	}
	drawer.RePaint( windowHandle );
	// если каретка показывалась до перерисовки, то отображаем ее
	if( caretShown ) {
		caret.Show();
	}
}

// Обработка изменения размера окна. пересчитывает некоторые свойства скроллов.
void CEditWindow::OnWmSize( LPARAM lParam ) 
{
	recalculateHorzScrollParams();
	recalculateVertScrollParams();
}

// Обработка сообщений о скроллировании.
void CEditWindow::OnWmHScroll( WPARAM wParam, LPARAM lParam )
{
	if( lParam == 0 ) {
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof( SCROLLINFO );
		scrollInfo.fMask = SIF_ALL;
		// меняем позицию скролла
		::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
		switch( LOWORD( wParam ) ) {
		case SB_LINELEFT:
			scrollInfo.nPos = max( scrollInfo.nPos - 1, scrollInfo.nMin );
			break;
		case SB_LINERIGHT:
			scrollInfo.nPos = min( scrollInfo.nPos + 1, scrollInfo.nMax );
			break;
		case SB_PAGELEFT:
			scrollInfo.nPos = max( scrollInfo.nPos - static_cast<int>( scrollInfo.nPage ), scrollInfo.nMin );
			break;
		case SB_PAGERIGHT:
			scrollInfo.nPos = min( scrollInfo.nPos + static_cast<int>( scrollInfo.nPage ), scrollInfo.nMax );
			break;
		case SB_THUMBTRACK:
			scrollInfo.nPos = scrollInfo.nTrackPos;
			break;
		}
		// запоминаем и устанавливаем новую позицию скролла
		int scrollPosition = scrollInfo.nPos;
		scrollInfo.fMask = SIF_POS;
		::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
		::SetScrollInfo( windowHandle, SB_HORZ, &scrollInfo, TRUE );
		// и перестраховка, на случай того, что Window сдвинет что-то не так, как ожидалось
		::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
		if( scrollInfo.nPos != scrollPosition ) {
			::ScrollWindow( windowHandle, horizontalScrollUnit * ( scrollPosition - scrollInfo.nPos ), 0, 0, 0 );
		}
	}
}

// Обработка сообщений о скроллировании.
void CEditWindow::OnWmVScroll( WPARAM wParam, LPARAM lParam )
{
	if( lParam == 0 ) {
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof( SCROLLINFO );
		scrollInfo.fMask = SIF_ALL;
		// меняем позицию скролла
		::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
		switch( LOWORD( wParam ) ) {
		case SB_LINEUP:
			scrollInfo.nPos = max( scrollInfo.nPos - 1, scrollInfo.nMin );
			break;
		case SB_LINEDOWN:
			scrollInfo.nPos = min( scrollInfo.nPos + 1, scrollInfo.nMax );
			break;
		case SB_PAGEUP:
			scrollInfo.nPos = max( scrollInfo.nPos - static_cast<int>( scrollInfo.nPage ), scrollInfo.nMin );
			break;
		case SB_PAGEDOWN:
			scrollInfo.nPos = min( scrollInfo.nPos + static_cast<int>( scrollInfo.nPage ), scrollInfo.nMax );
			break;
		case SB_THUMBTRACK:
			scrollInfo.nPos = scrollInfo.nTrackPos;
			break;
		}
		// запоминаем и устанавливаем новую позицию скролла
		int scrollPosition = scrollInfo.nPos;
		scrollInfo.fMask = SIF_POS;
		::RedrawWindow( windowHandle, 0, 0, RDW_INVALIDATE );
		::SetScrollInfo( windowHandle, SB_VERT, &scrollInfo, TRUE );

		// и перестраховка, на случай того, что Window сдвинет что-то не так, как ожидалось
		::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
		if( scrollInfo.nPos != scrollPosition ) {
			::ScrollWindow( windowHandle, 0, verticalScrollUnit * ( scrollPosition - scrollInfo.nPos ), 0, 0 );
		}
	}
}

// Метод, вызываемый при получении окном сообщения WM_LBUTTONDOWN.
void CEditWindow::OnLButDown( LPARAM lParam )
{
	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
	int moveX = scrollInfo.nPos * horizontalScrollUnit;
	::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
	int moveY = scrollInfo.nPos * verticalScrollUnit;

	int x = GET_X_LPARAM( lParam );
	int y = GET_Y_LPARAM( lParam );

	caret.MoveTo( x + moveX, y + moveY );
	symbolSelector.ResetSelection();
	symbolSelector.SetStartPosition( x + moveX, y + moveY, firstEnablePosition );
	HideCaret();
}

// Метод, вызываемый при получении окном сообщения WM_LBUTTONUP.
void CEditWindow::OnLButUp( LPARAM lParam )
{
	int x = GET_X_LPARAM( lParam );
	int y = GET_Y_LPARAM( lParam );

	SCROLLINFO scrollInfo;
		::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
		scrollInfo.cbSize = sizeof( SCROLLINFO );
		scrollInfo.fMask = SIF_ALL;

		::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
		int moveX = scrollInfo.nPos * horizontalScrollUnit;
		::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
		int moveY = scrollInfo.nPos * verticalScrollUnit;

	if( symbolSelector.HasSelection() ) {
		symbolSelector.SetCurrentPosition( x + moveX, y + moveY, firstEnablePosition );
		CSymbolPosition start = symbolSelector.GetSelectionInfo().first;
		if( start.Index == -1 || start.Index == start.CurrentLine->Length() ) {
			symbolSelector.ResetSelection();
			ShowCaret();
		}
	} else {
		caret.MoveTo( x + moveX, y + moveY );
		ShowCaret();
	}
	InvalidateRect( windowHandle, 0, true );
}

// Метод, вызываемый при получении окном сообщения WM_MOUSEMOVE.
void CEditWindow::OnLockedMouseMove( LPARAM lParam )
{
	if( symbolSelector.HasSelection() ) {
		HideCaret();
	} else {
		ShowCaret();
	}
	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
	int moveX = scrollInfo.nPos * horizontalScrollUnit;
	::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
	int moveY = scrollInfo.nPos * verticalScrollUnit;
	symbolSelector.SetCurrentPosition( GET_X_LPARAM( lParam ) + moveX, GET_Y_LPARAM( lParam ) + moveY, firstEnablePosition );
	InvalidateRect( windowHandle, 0, true );
}

// private методы.

// Удалить выделенные символы редактора.
void CEditWindow::removeSelectedItems()
{
	CItemSelector::CSymbolInterval interval = symbolSelector.GetSelectionInfo();
	CSymbolPosition start = interval.first;
	CSymbolPosition end = interval.second;
	if( start.Index != -1 && end.Index != -1 ) {
		if( start.GetParent() != 0 ) {
			removeLocalSelected( start, end );
		} else {
			removeGlobalSelected( start, end );
		}
	}
}

// Удалить символы, заданные позициями, в одной строке.
void CEditWindow::removeLocalSelected( const CSymbolPosition& start, const CSymbolPosition& end )
{
	CLineOfSymbols* currentLine = const_cast<CLineOfSymbols*>( start.CurrentLine );
	for( int i = end.Index; i >= start.Index; --i ) {
		currentLine->Delete( i );
	}
	caret.MoveTo( CSymbolPosition( start.Index, start ) );
}

// Удалить символы, заданные позициями, из редактора.
void CEditWindow::removeGlobalSelected( const CSymbolPosition& start, const CSymbolPosition& end )
{
	int startIdx = getBaseLineIndex( start.CurrentLine );
	int endIdx = getBaseLineIndex( end.CurrentLine );
	if( startIdx == endIdx ) {
		removeLocalSelected( start, end );
	} else {
		removeLocalSelected( start, CSymbolPosition( start.CurrentLine->Length() - 1, start ) );
		for( int i = startIdx + 1; i < endIdx; ++i ) {
			if( content[i].Length() != 0 ) {
				removeLocalSelected( CSymbolPosition( 0, &content[i] ), CSymbolPosition( content[i].Length() - 1, &content[i] ) );
			}
		}
		removeLocalSelected( CSymbolPosition( 0, end ), end );
		for( int i = 0; i < end.CurrentLine->Length(); ++i ) {
			content[startIdx].PushBack( content[endIdx][i]->Clone( &content[startIdx] ) );
		}
		content.erase( content.begin() + startIdx + 1, content.begin() + endIdx + 1 );
		caret.MoveTo( CSymbolPosition( start.Index, start ) );
	}
}

// Проверка валидности данных введеных в окне. В случае ошибки описание передается через ссылку.
bool CEditWindow::isInputValid( std::wstring& error ) const
{
	size_t i = 0;
	try {
		for( i = 0; i < content.size(); ++i ) {
			TestFormula( content[i].ToLatexString( firstEnablePosition ), knownVariables );
		}
	} catch( const std::runtime_error& e ) {
		std::string errorMsg( e.what() );
		error = std::to_wstring( i + 1 ) + std::wstring( L"-е определение функции невычислимо." ) + std::wstring( errorMsg.begin(), errorMsg.end() );
		return false;
	}
	return true;
}

// Допустим ли данный символ на ввод.
bool CEditWindow::isSymbolAllowed( wchar_t symbol ) const
{
	std::wstring allowedOperators = L"+-/*=().";
	return ( allowedOperators.find( symbol ) != std::wstring::npos || symbol == L' ' || ( symbol >= '0' && symbol <= '9' ) ||
		( symbol >= 'a'  &&  symbol <= 'z' ) || ( symbol >= 'A'  &&  symbol <= 'Z' ) );
}

// Получить номер строки в массиве. Возвращается -1 для строк, не являющися базовыми.
int CEditWindow::getBaseLineIndex( const CLineOfSymbols* line ) const
{
	for( size_t i = 0; i < content.size(); ++i ) {
		if( line == &content[i] ) {
			return i;
		}
	}

	return -1;
}

// Пересчет параметров, связанных со скролированием.
void CEditWindow::recalculateHorzScrollParams() const
{
	RECT clientRect;
	::GetClientRect( windowHandle, &clientRect );

	int width = 0;

	for( size_t i = 0; i < content.size(); ++i ) {
		width = max( content[i].GetWidth(), width );
	}

	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_HORZ, &scrollInfo );
	scrollInfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;

	scrollInfo.nMax = width / horizontalScrollUnit;
	scrollInfo.nPage = clientRect.right / horizontalScrollUnit;
	if( scrollInfo.nPos > scrollInfo.nMax ) {
		scrollInfo.nPos = scrollInfo.nMax;
	}

	::SetScrollInfo( windowHandle, SB_HORZ, &scrollInfo, TRUE );
}

// Пересчет параметров, связанных со скролированием.
void CEditWindow::recalculateVertScrollParams() const
{
	RECT clientRect;
	::GetClientRect( windowHandle, &clientRect );

	int height = 0;

	for( size_t i = 0; i < content.size(); ++i ) {
		height += content[i].GetHeight();
	}

	SCROLLINFO scrollInfo;
	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;

	::GetScrollInfo( windowHandle, SB_VERT, &scrollInfo );
	scrollInfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;

	scrollInfo.nMax = height / verticalScrollUnit;
	scrollInfo.nPage = clientRect.bottom / verticalScrollUnit;
	if( scrollInfo.nPos > scrollInfo.nMax ) {
		scrollInfo.nPos = scrollInfo.nMax;
	}

	::SetScrollInfo( windowHandle, SB_VERT, &scrollInfo, TRUE );
}

CEditWindow::CCaret::CCaret( CEditWindow* _window, const CLineOfSymbols* baseLine ) : caretPosition( 0, baseLine )
{
	window = _window;
	isShown = false;
	height = window->simpleSymbolHeight;
	width = 2;
}

void CEditWindow::CCaret::Create()
{
	::CreateCaret( window->windowHandle, 0, width, height );
	::SetCaretBlinkTime( 300 );
}

// Показать каретку.
void CEditWindow::CCaret::Show()
{
	moveToNewCoordinates();
	if( !isShown ) {
		isShown = ::ShowCaret( window->windowHandle ) != 0;
	}
}

// Скрыть каретку.
void CEditWindow::CCaret::Hide()
{
	if( isShown ) {
		::HideCaret( window->windowHandle );
		isShown = false;
	}
}

void CEditWindow::CCaret::Destroy()
{
	isShown = false;
	::DestroyCaret();
}

// Сдвигает каретку на единицу в данном направлении.
void CEditWindow::CCaret::Move( CEditWindow::TCaretDirection direction )
{
	switch( direction ) {
	case CD_Up:
		moveUp();
		break;
	case CD_Down:
		moveDown();
		break;
	case CD_Left:
		moveLeft();
		break;
	case CD_Right:
		moveRight();
		break;
	}
}

// Сдвигает каретку в определенную позицию.
void CEditWindow::CCaret::MoveTo( const CSymbolPosition& newPosition ) 
{
	CSymbolPosition tmp( newPosition );
	std::swap( caretPosition, tmp );
}

// Переместить каретку в соответствии с координатами в окне.
void CEditWindow::CCaret::MoveTo( int x, int y )
{
	std::unique_ptr<const CSymbolPosition> tmp( window->finder.FindPosition( x, y, window->firstEnablePosition ) );
	if( tmp->Index == -1 ) {
		MoveTo( CSymbolPosition( 0, *tmp ) );
	} else if( tmp->Index == tmp->CurrentLine->Length() ) {
		MoveTo( *tmp );
	} else {
		if( ( *tmp->CurrentLine )[tmp->Index]->GetX() + ( *tmp->CurrentLine )[tmp->Index]->GetWidth() / 2  > x ) {
			MoveTo( *tmp );
		} else {
			MoveTo( CSymbolPosition( tmp->Index + 1, *tmp ) );
		}
	}
}

// Изменить высоту каретки.
void CEditWindow::CCaret::changeHeight( int newHeight )
{
	bool wasShown = isShown;
	height = newHeight;
	Destroy();
	Create();
	if( wasShown ) {
		Show();
	}
}

// Передвижения каретки для каждого из 4-ех направлений.
void CEditWindow::CCaret::moveUp()
{
	if( caretPosition.GetParent() == 0 ) {
		int lineIdx = window->getBaseLineIndex( caretPosition.CurrentLine );
		if( lineIdx > 0 ) {
			caretPosition = CSymbolPosition( min( caretPosition.Index, window->content[lineIdx - 1].Length() ), &window->content[lineIdx - 1] );
		}
	}
	moveToNewCoordinates();
}

// Передвижения каретки для каждого из 4-ех направлений.
void CEditWindow::CCaret::moveDown()
{
	if( caretPosition.GetParent() == 0 ) {
		int lineIdx = window->getBaseLineIndex( caretPosition.CurrentLine );
		if( lineIdx != window->content.size() - 1 ) {
			caretPosition = CSymbolPosition( min( caretPosition.Index, window->content[lineIdx + 1].Length() ), &window->content[lineIdx + 1] );
		}
	}
	moveToNewCoordinates();
}

// Передвижения каретки для каждого из 4-ех направлений.
void CEditWindow::CCaret::moveLeft()
{
	if( caretPosition.Index > window->firstEnablePosition || ( caretPosition.GetParent() != 0 && caretPosition.Index > 0 ) ) {
		--caretPosition.Index;
		if( caretPosition.CurrentLine->Length() != caretPosition.Index ) {
			std::vector<const CLineOfSymbols*> tmpSubstrings;
			( *caretPosition.CurrentLine )[caretPosition.Index]->GetSubstrings( tmpSubstrings );
			if( tmpSubstrings.size() != 0 ) {
				std::unique_ptr<CSymbolPosition> newParent( new CSymbolPosition( caretPosition ) );
				caretPosition = CSymbolPosition( tmpSubstrings[tmpSubstrings.size() - 1]->Length(),
															   tmpSubstrings[tmpSubstrings.size() - 1], newParent.release() );
			}
		}
	} else if( caretPosition.GetParent() != 0 && caretPosition.Index == 0 ) {
		std::vector<const CLineOfSymbols*> tmpSubstrings;
		const CSymbolPosition* parent = caretPosition.GetParent();
		( *parent->CurrentLine )[parent->Index]->GetSubstrings( tmpSubstrings );
		int currentLineIdx;
		for( currentLineIdx = tmpSubstrings.size() - 1; currentLineIdx >= 0; --currentLineIdx ) {
			if( tmpSubstrings[currentLineIdx] == caretPosition.CurrentLine ) {
				break;
			}
		}
		if( --currentLineIdx >= 0 ) {
			caretPosition = CSymbolPosition( tmpSubstrings[currentLineIdx]->Length(), tmpSubstrings[currentLineIdx], new CSymbolPosition( *parent ) );
		} else {
			caretPosition = CSymbolPosition( *parent );
		}
	}
	moveToNewCoordinates();
}

// Передвижения каретки для каждого из 4-ех направлений.
void CEditWindow::CCaret::moveRight()
{
	if( caretPosition.CurrentLine->Length() > caretPosition.Index ) {
		std::vector<const CLineOfSymbols*> tmpSubstrings;
		( *caretPosition.CurrentLine )[caretPosition.Index]->GetSubstrings( tmpSubstrings );
		if( tmpSubstrings.size() == 0 ) {
			++caretPosition.Index;
		} else {
			std::unique_ptr<CSymbolPosition> newParent( new CSymbolPosition( caretPosition ) );
			caretPosition = CSymbolPosition( 0, tmpSubstrings[0], newParent.release() );
		}
	} else if( caretPosition.GetParent() != 0 ) {
		std::vector<const CLineOfSymbols*> tmpSubstrings;
		const CSymbolPosition* parent = caretPosition.GetParent();
		( *parent->CurrentLine )[parent->Index]->GetSubstrings( tmpSubstrings );
		size_t currentLineIdx = 0;
		for( ; currentLineIdx < tmpSubstrings.size(); ++currentLineIdx ) {
			if( tmpSubstrings[currentLineIdx] == caretPosition.CurrentLine ) {
				break;
			}
		}
		if( currentLineIdx + 1 == tmpSubstrings.size() ) {
			caretPosition = CSymbolPosition( *parent );
			++caretPosition.Index;
		} else {
			caretPosition = CSymbolPosition( 0, tmpSubstrings[currentLineIdx + 1], new CSymbolPosition( *parent ) );
		}
	}
	moveToNewCoordinates();
}

// Сместить каретку к заданной в ней позиции.
void CEditWindow::CCaret::moveToNewCoordinates()
{
	const CLineOfSymbols* caretLine = caretPosition.CurrentLine;
	int caretIndex = caretPosition.Index;
	int newHeight = caretLine->GetHeight();

	int x;
	int y = caretLine->GetY();
	if( caretLine->Length() == 0 ) {
		x = caretLine->GetX();
		newHeight = caretLine->GetHeight();
	} else if( caretIndex == 0 ) {
		x = ( *caretLine )[caretIndex]->GetX();
	} else {
		x = ( *caretLine )[caretIndex - 1]->GetX() + ( *caretLine )[caretIndex - 1]->GetWidth();
	}
	if( newHeight > 0 && newHeight != height ) {
		changeHeight( newHeight );
	}

	::SetCaretPos( x, y );
}

// Установить известные переменные в соответствии с типом функции.
void CEditWindow::setVariables()
{
	switch( currentFunctionType ) {
		case FT_YfromX:
			knownVariables.insert("x");
			break;
		case FT_XYfromT:
			knownVariables.insert("t");
			break;
		case FT_ZfromXY:
			knownVariables.insert("x");
			knownVariables.insert("y");
			break;
		case FT_XYZfromT:
			knownVariables.insert("t");
			break;
		case FT_XYZfromTL:
			knownVariables.insert("t");
			knownVariables.insert("l");
			break;
		default:
			assert( false );
	}
}

// Установить первую доступную позицию в строке.
void CEditWindow::setFirstEnablePosition()
{
	switch( currentFunctionType ) {
		case FT_YfromX:
			firstEnablePosition = 5;
			break;
		case FT_XYfromT:
			firstEnablePosition = 5;
			break;
		case FT_ZfromXY:
			firstEnablePosition = 7;
			break;
		case FT_XYZfromT:
			firstEnablePosition = 5;
			break;
		case FT_XYZfromTL:
			firstEnablePosition = 7;
			break;
		default:
			assert( false );
	}
}

// Установка базового ввода.
void CEditWindow::setBaseContent()
{
	switch( currentFunctionType ) {
		case FT_YfromX:
			insertOneParametrFunc( 'y', 'x' );
			break;
		case FT_XYfromT:
			insertOneParametrFunc( 'x', 't' );
			insertOneParametrFunc( 'y', 't' );
			break;
		case FT_ZfromXY:
			insertTwoParametrFunc( 'z', 'x', 'y' );
			break;
		case FT_XYZfromT:
			insertOneParametrFunc( 'x', 't' );
			insertOneParametrFunc( 'y', 't' );
			insertOneParametrFunc( 'z', 't' );
			break;
		case FT_XYZfromTL:
			insertTwoParametrFunc( 'x', 't', 'l' );
			insertTwoParametrFunc( 'y', 't', 'l' );
			insertTwoParametrFunc( 'z', 't', 'l' );
			break;
		default:
			assert( false );
	}
}

// Вставка в массив строк явных описаний функций.
void CEditWindow::insertOneParametrFunc( wchar_t funcName, wchar_t parametrName )
{
	content.push_back( CLineOfSymbols( simpleSymbolHeight, true ) );
	size_t i = content.size() - 1;
	content[i].PushBack( new CSimpleSymbol( funcName ) );
	content[i].PushBack( new CSimpleSymbol( '(' ) );
	content[i].PushBack( new CSimpleSymbol( parametrName ) );
	content[i].PushBack( new CSimpleSymbol( ')' ) );
	content[i].PushBack( new CSimpleSymbol( '=' ) );
}

// Вставка в массив строк явных описаний функций.
void CEditWindow::insertTwoParametrFunc( wchar_t funcName, wchar_t firstParametrName, wchar_t secondParametrName )
{
	content.push_back( CLineOfSymbols( simpleSymbolHeight, true ) );
	size_t i = content.size() - 1;
	content[i].PushBack( new CSimpleSymbol( funcName ) );
	content[i].PushBack( new CSimpleSymbol( '(' ) );
	content[i].PushBack( new CSimpleSymbol( firstParametrName ) );
	content[i].PushBack( new CSimpleSymbol( ',' ) );
	content[i].PushBack( new CSimpleSymbol( secondParametrName ) );
	content[i].PushBack( new CSimpleSymbol( ')' ) );
	content[i].PushBack( new CSimpleSymbol( '=' ) );
}

// Запись в файл выделенных символов.
bool CEditWindow::writeSelectedInFile( LPWSTR fileName, int fileExtentionPos ) const
{
	HANDLE file = ::CreateFile( fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	CLineOfSymbols tmp( simpleSymbolHeight );
	CItemSelector::CSymbolInterval selectedInterval = symbolSelector.GetSelectionInfo();
	CSymbolPosition start = selectedInterval.first;
	CSymbolPosition end = selectedInterval.second;
	for( int i = start.Index; i <= end.Index; ++i ) {
		tmp.PushBack( ( *start.CurrentLine )[i]->Clone( &tmp ) );
	}
	std::string outputString = tmp.ToLatexString();
	TSupportedFormats format = SF_LATEX;
	if( fileName + fileExtentionPos == std::wstring( L"xml" ) ) {
		format = SF_OPENMATH;
	} else if( fileName + fileExtentionPos == std::wstring( L"mml" ) ) {
		format = SF_MATHML;
	}
	DWORD countBytes = 0;
	::WriteFile( file, outputString.c_str(), outputString.size(), &countBytes, 0 );
	::CloseHandle( file );
	if( format != SF_LATEX ) {
		std::wstring input( fileName );
		std::string inputAsString( input.begin(), input.end() );
		ConvertFormula( inputAsString, SF_LATEX, format, inputAsString );
	}
	return outputString.size() == countBytes;
}

// Чтение из выбранного файла для импорта формулы.
bool CEditWindow::readSelectedFromFile( LPWSTR fileName, int fileExtentionPos ) const {
    TSupportedFormats format = SF_LATEX;
    if( fileName + fileExtentionPos == std::wstring( L"xml" ) ) {
        format = SF_OPENMATH;
    }
    else if( fileName + fileExtentionPos == std::wstring( L"mml" ) ) {
        format = SF_MATHML;
    }

    std::wstring input( fileName );
    std::string inputAsString( input.begin( ), input.end( ) );
    shared_ptr<MathObj> tree = ConvertToTree( inputAsString, format );

    CLineOfSymbols tmp( simpleSymbolHeight, tree );

    return true;
}



// Стандартная процедура обработки сообщений окном.
LRESULT __stdcall CEditWindow::windowProcedure( HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_NCCREATE ) {
		::SetWindowLong( windowHandle, GWL_USERDATA, reinterpret_cast<LONG>( reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams ) );
	}

	CEditWindow* window = reinterpret_cast<CEditWindow*>( ::GetWindowLong( windowHandle, GWL_USERDATA ) );

	switch( message ) {
	case WM_REDACTOR_OK:
		::SendMessage( window->parentHandle, WM_REDACTOR_OK, 0, 0 );
		return 0;
	case WM_CLOSE:
		window->OnWmDestroy();
		break;
	case WM_PAINT:
		window->OnWmPaint();
		break;
	case WM_HSCROLL:
		window->OnWmHScroll( wParam, lParam );
		break;
	case WM_VSCROLL:
		window->OnWmVScroll( wParam, lParam );
		break;
	case WM_SIZE:
		window->OnWmSize( lParam );
		break;
	case WM_LBUTTONDOWN:
		window->OnLButDown( lParam );
		break;
	case WM_MOUSEMOVE:
		if( wParam & MK_LBUTTON ) {
			window->OnLockedMouseMove( lParam );
		}
		break;
	case WM_LBUTTONUP:
		window->OnLButUp( lParam );
		break;
	}
	return ::DefWindowProc( windowHandle, message, wParam, lParam );
}