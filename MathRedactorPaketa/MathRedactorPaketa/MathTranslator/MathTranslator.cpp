// MathTranslator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <Windows.h>
#include "MathFormObj.h"
#include "ConvertOM.h"
#include "MathML.h"
#include "ConvertLatex.h"

// ConverterMemory.cpp
//extern HANDLE ConverterHeap;

void MTConvert( char* inputParam, char* inputFileName, char* outputParam, char* outputFileName )
{
	shared_ptr<FormulaObj> obj(new FormulaObj(NT_MAIN));
	MathMLParser mmlparser;
	CConvertLatex latexConverter;
	shared_ptr<MathObj> latexObj;

	std::string inputPar( inputParam );

	if( inputPar == "omath" )
	{
		ConvertFromOM( inputFileName, obj );
	}
	if( inputPar == "mathml" )
	{
		mmlparser.Pars( inputFileName );
	}

	if( inputPar == "latex" ) {
		ifstream ifs( inputFileName );
		string str( ( istreambuf_iterator<char>( ifs ) ), istreambuf_iterator<char>() );
		latexObj = latexConverter.ConvertFromLatex( str );
		ifs.close();
	}
	std::string outputPar( outputParam );
	if( outputPar == "omath" )
	{
		if( inputPar == "omath" )
		{
			ConvertToOM( outputFileName, obj );
		}
		if( inputPar == "mathml" )
		{
			ConvertToOM( outputFileName, mmlparser.GetData() );
		}

		if( inputPar == "latex" ) {
			ConvertToOM( outputFileName, latexObj );
		}
	}
    if( outputPar == "mathml" )
    {
        if( inputPar == "omath" )
        {
            mmlparser.SetData( obj );
            mmlparser.Save( outputFileName );
        }
        if( inputPar == "mathml" )
            mmlparser.Save( outputFileName );

		if( inputPar == "latex" ) {
			mmlparser.SetData( latexObj );
			mmlparser.Save( outputFileName );
		}
    }
	if( outputPar == "latex" ) {
		string outStr;
		if( inputPar == "latex" ) {
			latexConverter.ConvertToLatex( latexObj, outStr );
		} else if (inputPar == "omath") {
			latexConverter.ConvertToLatex( obj, outStr );
		} else if (inputPar == "mathml") { 
			latexConverter.ConvertToLatex( mmlparser.GetData(), outStr);
		}
		ofstream ofs( outputFileName, ios::out );
		ofs << outStr;
		ofs.close();
	}
}

//void MTRead( char* param, char* inputFileName, MathObj* obj ) 
//{
//	std::string par( param );
//	if( par == "omath" ) 
//    {
//		ConvertFromOM( inputFileName, obj );
//	}
//	if( par == "mathml" )
//	{
//		MathMLParser* A = new MathMLParser();
//		A->Pars( inputFileName );
//		obj = A->GetData();
//	}
//	
//}

//void MTWrite( char* param, char* outputFileName, MathObj* obj ) 
//{
//	std::string par( param );
//	if( par == "omath" ) 
//    {
//		ConvertToOM( outputFileName, obj );
//	}
//
//    if( par == "mathml" )
//    {
//        MathMLParser* A = new MathMLParser();
//        A->SetData( obj );
//        A->Save( outputFileName );
//    }
//}

int main() {
	MTConvert("latex", "inputLatex1.txt", "latex", "output");
	return 0;
}