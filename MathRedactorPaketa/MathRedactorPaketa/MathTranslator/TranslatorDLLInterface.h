#include <string>
//поддерживаемые форматы конвентора
enum TSupportedFormats {
	SF_LATEX,
	SF_MATHML,
	SF_OPENMATH
};

#ifdef MATHTRANSLATORDLL_EXPORTS
#define MATHFUNCSDLL_API __declspec(dllexport) 
#else
#define MATHFUNCSDLL_API __declspec(dllimport) 
#endif

void MATHFUNCSDLL_API convertFormula(std::string inputFile, TSupportedFormats inputFormat, TSupportedFormats outputFormat, std::string outputFile);
//void __declspec(dllexport) convertFormula(std::string inputFile, TSupportedFormats inputFormat, TSupportedFormats outputFormat);
