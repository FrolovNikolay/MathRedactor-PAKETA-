// Автор: Николай Левшин 
// Назначение: Инициализация класса MathObj, который используется как внутренний формат для конвертации данных

#include "MathFormObj.h"

FormulaObj::FormulaObj() {}

FormulaObj::FormulaObj( TNodeType t ) : type(t) 
{
	params.clear(); 
}

ParamObj::ParamObj() {}

ParamObj::ParamObj( std::string s ) : val(s) {}
