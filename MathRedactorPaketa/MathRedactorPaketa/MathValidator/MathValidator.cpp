#define MATHVALIDDLL_EXPORTS

#include "MathValidator.h"
#include <NotationTester.h>
#include <ErrorCatcher.h>
#include <exception>

void TestFormula( const std::string& str, const std::set<std::string>& knownVars )
{
	CNotationTester notationTester;
	try {
		notationTester.Test( str, knownVars );
	} catch( const CErrorCatcher& e ) {
		throw std::runtime_error( e.what() );
	}
}