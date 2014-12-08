// Автор: Николай Левшин
/* Описание: Здесь лежит определение таблиц аттрибутов операторов в OpenMath */

#include <map>
#include <string>

//IdCollection - какие словари и операторы в общем формате MathObj соотвествуют оператору из OpenMath
std::map<std::string, std::pair<std::string, int>> ids = { { "plus", { "arith1", 1 } },
                                                            { "unary_minus", { "arith1", 2 } },
                                                            { "minus", { "arith1", 3 } },
                                                            { "times1", { "arith1", 4 } },
                                                            { "divide", { "arith1", 5 } },
                                                            { "power", { "arith1", 6 } },
                                                            { "abs", { "arith1", 7 } },
                                                            { "root", { "arith1", 8 } },
                                                            { "sum", { "arith1", 9 } },
                                                            { "product", { "arith1", 10 } },
                                                            { "times2", { "arith2", 11 } },
                                                            { "inverse", { "arith2", 12 } },
                                                            { "sin", { "transc1", 16 } },
                                                            { "cos", { "transc1", 17 } },
                                                            { "tan", { "transc1", 18 } },
                                                            { "cot", { "transc1", 21 } } };

//AttrCollection - какой оператор и словарь в OpenMath соответствует оператору из общего формата MathObj
std::map<int, std::pair<std::string, std::string>> attrs = { { 1, { "arith1", "plus" } },
                                                            { 2, { "arith1", "unary_minus" } },
                                                            { 3, { "arith1", "minus" } },
                                                            { 4, { "arith1", "times1" } },
                                                            { 5, { "arith1", "divide" } },
                                                            { 6, { "arith1", "power" } },
                                                            { 7, { "arith1", "abs" } },
                                                            { 8, { "arith1", "root" } },
                                                            { 9, { "arith1", "sum" } },
                                                            { 10, { "arith1", "product" } },
                                                            { 11, { "arith2", "times2" } },
                                                            { 12, { "arith2", "inverse" } },
                                                            { 16, { "trancs1", "sin" } },
                                                            { 17, { "transc1", "cos" } },
                                                            { 18, { "transc1", "tan" } },
                                                            { 19, { "transc1", "cot" } } };