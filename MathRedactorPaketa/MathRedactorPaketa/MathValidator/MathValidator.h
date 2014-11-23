// Автор: Хусаенов Тимур.

// Описание: определение импортируемых из MathValidator.dll фунций.

#pragma once

#include <string>
#include <set>

#ifdef MATHVALIDDLL_EXPORTS
#define MATHVALIDDLL_API __declspec(dllexport) 
#else
#define MATHVALIDDLL_API __declspec(dllimport) 
#endif

// Проверка заданной формулы на валидность.
void MATHVALIDDLL_API TestFormula( const std::string&, const std::set<std::string>& );
