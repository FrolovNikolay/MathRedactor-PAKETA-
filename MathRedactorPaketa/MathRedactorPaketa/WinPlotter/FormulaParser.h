// Автор: Федюнин Валерий
// Описание: распознавание текстового записи формулы в объект класса CFormula

#pragma once

#include <string>

#include "CFormula.h"

// распознает формулу, записанную в строке
CFormula ParseFormula( const std::string& text );