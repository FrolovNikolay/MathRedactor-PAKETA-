#include "MathValidator.h"
#include <NotationTester.h>

extern "C" bool testFormula( std::string str )
{
	CNotationTester notationTester;
	return notationTester.Test( str );
}