#define MATHVALIDDLL_EXPORTS

#include "MathValidator.h"
#include <NotationTester.h>

bool TestFormula( const std::string& str )
{
	CNotationTester notationTester;
	return notationTester.Test( str );
}