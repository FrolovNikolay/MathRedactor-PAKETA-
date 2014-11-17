// Автор: Николай Левшин 
// Назначение: Объявление класса MathObj, который используется как внутренний формат для конвертации данных

#include <vector>
#include <string>
#include "tinyxml.h"
#include <memory>
#pragma once

using namespace std;

enum TNodeType { 
    NT_PLUS = 1,    // Плюс
    NT_UMINUS = 2,  //унарный минус
    NT_MINUS = 3,   //минус
    NT_MULTNCM = 4, //не коммутативное умножение
    NT_DIV = 5,     //деление
    NT_POW = 6,     //возведение в степень, первый потомок - основание
    NT_ABS = 7,     //взятие модуля
    NT_ROOT = 8,    //корень n-ой степени, первый потомок - степень, если потомок один, то по умолчанию квадратный корень
    NT_SUM = 9,     //оператор суммирования
    NT_PROD = 10,   //оператор перемножения, как П

    NT_MULTCM = 11, //коммутативное умножение
    NT_INVERSE = 12,//взятие обратного элемента

    NT_LOG = 13,    //взятие логарифма, первый потомок - база, второй - то, что логарифмируют
    NT_LN = 14,     //взятие натурального логарифма
    NT_EXP = 15,    //
    NT_SIN = 16,    //  ##########   #########   ###      #####   #########
    NT_COS = 17,    //  ##########   #########   ###     ## ###   #########
    NT_TAN = 18,    //      ###      ###   ###   ###    ##  ###   ###
    NT_SEC = 19,    //      ###      #########   ###   ##   ###   ###          ########
    NT_CSC = 20,    //      ###      #########   ###  ##    ###   ###          ########
    NT_COT = 21,    //      ###      ###         ### ##     ###   ###
    NT_SINH = 22,   //      ###      ###         #####      ###   ###
    NT_COSH = 23,   //
    NT_TANH = 24,   //   #########   ###   ###   #########   ####       ####
    NT_SECH = 25,   //   #########   ###   ###   #########   #####     #####
    NT_CSCH = 26,   //   ###   ###   ###   ###   ###   ###   ### ##   ## ###
    NT_COTH = 27,   //   ###   ###   #########   ###   ###   ###  ## ##  ###   ########
    NT_ARCSIN = 28, //   ###   ###   #########   ###   ###   ###   ###   ###   ########
    NT_ARCCOS = 29, //   #########   ###   ###   #########   ###         ###
    NT_ARCTAN = 30, //   #########   ###   ###   #########   ###         ###
    NT_ARCSEC = 31, //  
    NT_ARCCSC = 32, //   #########   #########   #########   ###      #####     #######
    NT_ARCCOT = 33, //   #########   #########   #########   ###     ## ###   #########
    NT_ARCSINH = 34,//   ###            ###      ###   ###   ###    ##  ###   ###   ###
    NT_ARCCOSH = 35,//   #########      ###      #########   ###   ##   ###   #########
    NT_ARCTANH = 36,//   ###            ###      #########   ###  ##    ###    ########
    NT_ARCSECH = 37,//   #########      ###      ###         ### ##     ###    ###  ###
    NT_ARCCSCH = 38,//   #########      ###      ###         #####      ###   ###   ###        
    NT_ARCCOTH = 39,//

    NT_EQUAL = 40,  // равно
    NT_LESS = 41,   // меньше
    NT_GREAT = 42,  // больше
    NT_NEQUAL = 43, // не равно
    NT_LESSEQ = 44, // меньше или равно
    NT_GREATEQ = 45,// больше или равно
    NT_APPROX = 46, // примерно равно

    NT_PLUSMINUS = 47, // плюсминус

	NT_MAIN,        //Идентификатор того, что это корень нашего MathObj
	NT_NOTYPE,      //Безтипная вершина
}; //Тип оператора в вершине дерева разбора формулы


//Основной тип формулы
class MathObj {
public:
	virtual ~MathObj() {};
};

class FormulaObj : public MathObj {
private:
	TNodeType type = NT_NOTYPE;
public:
	std::vector< shared_ptr<MathObj> > params;

	FormulaObj();
	FormulaObj(TNodeType);

	TNodeType GetType();
	void SetType(TNodeType);
};

class ParamObj : public MathObj {
private:
	std::string val;
public:
	ParamObj();
	ParamObj(std::string);
	void SetVal(std::string s){ val = s; }

	std::string GetVal();
};


