// Автор: Николай Левшин 
// Назначение: Инициализация класса MathObj, который используется как внутренний формат для конвертации данных

#include "MathFormObj.h"

FormulaObj::FormulaObj() {}

FormulaObj::FormulaObj( TNodeType t ) : type(t) 
{
	params.clear(); 
}

TNodeType FormulaObj::GetType() 
{ 
	return type; 
}

void FormulaObj::SetType( TNodeType t ) 
{
	type = t;
}

ParamObj::ParamObj() {}

ParamObj::ParamObj( std::string s ) : val(s) {}

std::string ParamObj::GetVal() 
{ 
	return val; 
}