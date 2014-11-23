#include "SimpleSymbol.h"
#include <assert.h>
#include <wingdi.h>

void CSimpleSymbol::Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const
{
	//Отрисовка (выводим один символ из строки symbol)
	::TextOut( displayHandle, posX, posY, &symbol, 1 );

	//Обновление служебной информации
	x = posX;
	y = posY;
	SIZE fontSizeStruct;
	::GetTextExtentPoint32( displayHandle, &symbol, 1, &fontSizeStruct );
	width = fontSizeStruct.cx + SideOffset;
	height = simpleSymbolHeight;
}

int CSimpleSymbol::CalculateWidth( HDC displayHandle ) const
{
	SIZE fontSizeStruct;
	::GetTextExtentPoint32( displayHandle, &symbol, 1, &fontSizeStruct );
	width = fontSizeStruct.cx + SideOffset;
	return width;
}

// Преобразование символа, в необходимый плоттеру формат.
std::string CSimpleSymbol::ToPlotterString() const
{
	if( symbol == ' ') {
		return "";
	}
	char tmp[2];
	tmp[1] = '\0';
	::WideCharToMultiByte( CP_ACP, 0, &symbol, -1, tmp, 1, 0, 0);
	return tmp;
}