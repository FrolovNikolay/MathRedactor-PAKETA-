#pragma once

#include <string>
#include "MathFormObj.h"

// Поддерживаемые форматы конвентора.
enum TSupportedFormats {
	SF_LATEX,
	SF_MATHML,
	SF_OPENMATH
};

#ifdef MATHTRANSLATORDLL_EXPORTS
#define MATHTRANSLATORDLL_API __declspec(dllexport) 
#else
#define MATHTRANSLATORDLL_API __declspec(dllimport) 
#endif

void MATHTRANSLATORDLL_API ConvertFormula( std::string inputFile, TSupportedFormats inputFormat, TSupportedFormats outputFormat, std::string outputFile );
shared_ptr<MathObj> MATHTRANSLATORDLL_API ConvertToTree( std::string inputFileName, TSupportedFormats inputFormat );

