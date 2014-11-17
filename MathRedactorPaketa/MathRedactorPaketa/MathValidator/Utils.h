// Автор: Николай Фролов

// Описание: набор вспомогательных функций.
#pragma once 

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

// Разбиение строки в набор упорядоченных токенов.
std::vector<std::string> ConsturctTokens( const std::string& src );

bool IsAggregationFunction( const std::string& src );