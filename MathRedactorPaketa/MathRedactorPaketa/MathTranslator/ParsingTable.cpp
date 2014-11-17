// Автор: Азат Давлетшин
// Описание: Таблица shift, reduce переходов для LR-парсера. Построена с помощью Bison. В файле parser.y описана грамматика.
// В файле parser.bison.txt отладочный вывод Bison'а, на основе которого построена таблица

#include <vector>

using namespace std;

extern const int Err = INT_MAX;

// Таблица состояний. Если число положительное то происходит свдиг (shift) и переход в состояние, соответствующее этому числу.
// Если число отрицательное, то делаем свертку (reduce) по правилу, соотвествующему этому числу. Если встретили Err, то синтаксическая ошибка.

vector<vector<int> > ParsingTable = { /*        *,    +,    -,    [,    ],    ^,    _,    {,    }, PROD,  SUM,  COT,  TAN,  COS,  SIN,   ID, FRAC, SQRT,    (,    ),  Exp,  EOF */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   12,  Err },  /* 0 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   13,  Err },  /* 1 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,   14,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 2 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,   15,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 3 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   16,  Err },  /* 4 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   17,  Err },  /* 5 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   18,  Err },  /* 6 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   19,  Err },  /* 7 состояние */
											{  -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6 },  /* 8 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,   20,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 9 состояние */
											{ Err,  Err,  Err,   22,  Err,  Err,  Err,   21,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 10 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   23,  Err },  /* 11 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,   24 },  /* 12 состояние */
											{  -5,   -5,   -5,   -5,   -5,   28,   -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5 },  /* 13 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,   29,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 14 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,   30,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 15 состояние */
											{  27,  -16,  -16,  -16,  -16,   28,  -16,  -16,  -16,  -16,  -16,  -16,  -16,  -16,  -16,  -16,  -16,  -16,  -16,  -16,  -16,  -16 },  /* 16 состояние */
											{  27,  -15,  -15,  -15,  -15,   28,  -15,  -15,  -15,  -15,  -15,  -15,  -15,  -15,  -15,  -15,  -15,  -15,  -15,  -15,  -15,  -15 },  /* 17 состояние */
											{  27,  -14,  -14,  -14,  -14,   28,  -14,  -14,  -14,  -14,  -14,  -14,  -14,  -14,  -14,  -14,  -14,  -14,  -14,  -14,  -14,  -14 },  /* 18 состояние */
											{  27,  -13,  -13,  -13,  -13,   28,  -13,  -13,  -13,  -13,  -13,  -13,  -13,  -13,  -13,  -13,  -13,  -13,  -13,  -13,  -13,  -13 },  /* 19 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   31,  Err },  /* 20 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   32,  Err },  /* 21 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   33,  Err },  /* 22 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,   34,  Err,  Err },  /* 23 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 24 состояние (не существует) */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   35,  Err },  /* 25 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   36,  Err },  /* 26 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   37,  Err },  /* 27 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,   38,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 28 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   39,  Err },  /* 29 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   40,  Err },  /* 30 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,   41,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 31 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,   42,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 32 состояние */
											{  27,   25,   26,  Err,   43,   28,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 33 состояние */
											{ -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10 },  /* 34 состояние */
											{  27,   -2,   -2,   -2,   -2,   28,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2 },  /* 35 состояние */
											{  27,   -4,   -4,   -4,   -4,   28,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4 },  /* 36 состояние */
											{  -1,   -1,   -1,   -1,   -1,   28,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1 },  /* 37 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   44,  Err },  /* 38 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,   45,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 39 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,   46,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 40 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,   47,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 41 состояние */
											{  -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8 },  /* 42 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,   48,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 43 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,   49,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 44 состояние */
											{ Err,  Err,  Err,  Err,  Err,   50,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 45 состояние */
											{ Err,  Err,  Err,  Err,  Err,   51,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 46 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   52,  Err },  /* 47 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   53,  Err },  /* 48 состояние */
											{  -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7 },  /* 49 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,   54,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 50 состояние */
											{ Err,  Err,  Err,  Err,  Err,  Err,  Err,   55,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 51 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,   56,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 52 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,   57,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 53 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   58,  Err },  /* 54 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   59,  Err },  /* 55 состояние */
											{  -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3 },  /* 56 состояние */
											{  -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9,   -9 },  /* 57 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,   60,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 58 состояние */
											{  27,   25,   26,  Err,  Err,   28,  Err,  Err,   61,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err,  Err },  /* 59 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   62,  Err },  /* 60 состояние */
											{ Err,  Err,    1,  Err,  Err,  Err,  Err,  Err,  Err,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,  Err,   63,  Err },  /* 61 состояние */
											{  27,  -12,  -12,  -12,  -12,   28,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12 },  /* 62 состояние */
											{  27,  -11,  -11,  -11,  -11,   28,  -11,  -11,  -11,  -11,  -11,  -11,  -11,  -11,  -11,  -11,  -11,  -11,  -11,  -11,  -11,  -11 }   /* 62 состояние */
};