#ifdef MATHVALIDDLL_EXPORTS
#define MATHVALIDDLL_API __declspec(dllexport) 
#else
#define MATHVALIDDLL_API __declspec(dllimport) 
#endif
#include <string>

extern "C" MATHVALIDDLL_API bool testFormula( std::string );
