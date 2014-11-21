// Автор: Фролов Николай.

// Описание: набор вспомогательных межклассовых функций.

#pragma once

#include "LineOfSymbols.h"

// проверям попадает ли данная точка в данную линию
bool IsLineContainPoint( const CLineOfSymbols* line, int x, int y );