//Автор: Орлов Никита
#pragma once
#include <queue>
#include <utility>
#include "evaluate.h"
#include <sstream>
#include <regex>
#include <vector>

//первая строка формула, вторая отрезок параметров в виде аргумент 
//<var>=[<left>;<right>]'\n'
bool CGraphBuilder::buildPointGrid( const std::string& data )
{
	try {
		std::stringstream ss;
		for( int i = 0; i < data.size(); i++ ) {
			if( data[i] != '\r' ) {
				ss << data[i];
			}
		}
		std::string strFormula, argument;
		std::map<char, std::pair< double, double > > args;
		std::getline( ss, strFormula, '\n' );
		std::regex argRegex( "((t|l|x|y|z)=\\[([\\-+]?[0-9]*\\.?[0-9]+)(;)([\\-+]?[0-9]*\\.?[0-9]+)\\])" );
		std::regex epsRegex( "(eps=([0-9]*\\.?[0-9]+))" );
		while( std::getline( ss, argument ) ) {
			if( std::regex_match( argument, argRegex ) ) {
				int firstArgEnd = 3;
				while( argument[firstArgEnd] != ';' ) {
					firstArgEnd++;
				}
				int secondArgEnd = firstArgEnd;
				while( argument[secondArgEnd] != ']' ) {
					secondArgEnd++;
				}
				double lb = std::stod( argument.substr( 3, firstArgEnd - 3 ) ); //left bound from arg template
				double rb = std::stod( argument.substr( firstArgEnd + 1, secondArgEnd - firstArgEnd ) ); //right bound arg template
				if( lb > rb ) { //опасный момент нужно ли производить сравнение с eps?
					return false;
				}
				args[argument[0]] = std::make_pair( lb, rb );
			} else if( std::regex_match( argument, epsRegex ) ) {
				std::string strEps = argument.substr( 4 );
				this->eps = std::stod( strEps );
			} else {
				return false;
			}
		}

		CFormula formula = ParseFormula( strFormula );
		std::vector< char > vars = formula.GetVariables();
		for( int i = 0; i < vars.size(); i++ ) {
			if( args.find( vars[i] ) == args.end() ) {
				args[vars[i]] = std::make_pair( leftBound, rightBound );
			}
		}

		std::map< char, double > argToCount;
		if( vars.size() == 1 ) { 
			for( int i = 0; i < ( args[vars[0]].second - args[vars[0]].first ) / eps; i++ ) {
				argToCount[vars[0]] = args[vars[0]].first + i * eps;
				auto result = formula.Calculate( argToCount );
				points.push_back( C3DPoint( result['x'], result['y'], result['z'] ) );
				if( points.size() > 2 ) {
					segments.push_back( std::make_pair( points.size() - 1, points.size() - 2 ) );
				}
			}
		} else if( vars.size() == 2 ) {//TODO сделать циклы для argToCount
			int secondAxisSize = ( args[vars[1]].second - args[vars[1]].first ) / eps; //не очень красиво вносить это в циклы.
			int firstAxisSize = ( args[vars[0]].second - args[vars[0]].first ) / eps;
			for( int i = 0; i < firstAxisSize; i++ ) {
				for( int j = 0; j < secondAxisSize; j++ ) {
					argToCount[vars[0]] = args[vars[0]].first + i * eps;
					argToCount[vars[1]] = args[vars[1]].first + j * eps;
					auto result = formula.Calculate( argToCount );
					points.push_back( C3DPoint( result['x'], result['y'], result['z'] ) );
					if( j > 0 ) { //соединили соседние точки на одной оси
						segments.push_back( std::make_pair( points.size() - 1, points.size() - 2 ) );
					}
					if( i > 0 ) { //соединили соседние точки на другой оси
						segments.push_back( std::make_pair( points.size() - 1, points.size() - secondAxisSize - 1 ) );
					}
				}
			}
			
		} else {
			return false;
		}

	} catch( std::exception& e ) {
		return false;
	}

	return true;
}