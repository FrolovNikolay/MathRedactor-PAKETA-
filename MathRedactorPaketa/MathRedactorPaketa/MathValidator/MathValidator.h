// Автор: Хусаенов Тимур.

// Описание: определение импортируемых из MathValidator.dll фунций.

#pragma once

#include <string>

#ifdef MATHVALIDDLL_EXPORTS
#define MATHVALIDDLL_API __declspec(dllexport) 
#else
#define MATHVALIDDLL_API __declspec(dllimport) 
#endif

// Проверка заданной формулы на валидность.
bool MATHVALIDDLL_API TestFormula( const std::string& );
