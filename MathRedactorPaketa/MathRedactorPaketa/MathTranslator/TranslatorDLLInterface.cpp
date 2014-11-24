#define MATHTRANSLATORDLL_EXPORTS

#include "TranslatorDLLInterface.h"
#include <string>
#include <assert.h> 
#include "MathML.h"
#include "MathFormObj.h"
#include "ConvertOM.h"
#include <memory>
#include <fstream>  
#include "ConvertLatex.h"

void ConvertFormula( std::string inputFileName, TSupportedFormats inputFormat, TSupportedFormats outputFormat, std::string outputFileName) 
{
		shared_ptr<FormulaObj> obj(new FormulaObj(NT_MAIN));
		MathMLParser mmlparser;
		CConvertLatex latexConverter;
		shared_ptr<MathObj> latexObj;

		if (inputFormat == SF_OPENMATH) {
			ConvertFromOM(inputFileName, obj);
		}
		if (inputFormat == SF_MATHML) {
			mmlparser.Pars(inputFileName);
		}

		if (inputFormat == SF_LATEX) {
			ifstream ifs(inputFileName);
			string str((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
			latexObj = latexConverter.ConvertFromLatex(str);
			ifs.close();
		}
		if (outputFormat == SF_OPENMATH) {
			if (inputFormat == SF_OPENMATH) {
				ConvertToOM(outputFileName, obj);
			}
			if (inputFormat == SF_MATHML) {
				ConvertToOM(outputFileName, mmlparser.GetData());
			}

			if (inputFormat == SF_LATEX) {
				ConvertToOM(outputFileName, latexObj);
			}
		}
		if (outputFormat == SF_MATHML) {
			if (inputFormat == SF_OPENMATH) {
				mmlparser.SetData(obj);
				mmlparser.Save(outputFileName);
			}
			if (inputFormat == SF_MATHML)
				mmlparser.Save(outputFileName);

			if (inputFormat == SF_LATEX) {
				mmlparser.SetData(latexObj);
				mmlparser.Save(outputFileName);
			}
		}
		if (outputFormat == SF_LATEX) {
			string outStr;
			if (inputFormat == SF_LATEX) {
				latexConverter.ConvertToLatex(latexObj, outStr);
			} else if (inputFormat == SF_OPENMATH) {
				latexConverter.ConvertToLatex(obj, outStr);
			} else if (inputFormat == SF_MATHML) {
				latexConverter.ConvertToLatex(mmlparser.GetData(), outStr);
			}
			ofstream ofs(outputFileName, ios::out);
			ofs << outStr;
			ofs.close();
		}
}