// Автор: Тимур Хусаенов.

// Описание: Класс исключений со специальными сообщениями об ошибке для валидатора.

#pragma once

#include <string>
#include <vector>

class CErrorCatcher
{
public:
	CErrorCatcher( std::string message, const std::vector<std::string>& tokens, int index );
	std::string what() { return msg + " in: " + subStr; }
private:
	std::string msg;
	std::string subStr;
};
