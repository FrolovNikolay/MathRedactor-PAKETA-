//Àâòîð: Îðëîâ Íèêèòà
#pragma once
#include <queue>
#include <utility>
#include "evaluate.h"
#include <sstream>
#include <regex>
#include <vector>


bool CGraphBuilder::buildPointGrid( const CFormula& formula, std::map< char, std::pair< double, double > > args, double eps )
{
	try {
		points.clear();
		segments.clear();
		std::vector<char> vars = formula.GetVariables();

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
			// не очень красиво вносить это в циклы.
			int secondAxisSize = static_cast<int>( ( args[vars[1]].second - args[vars[1]].first ) / eps ); 
			int firstAxisSize = static_cast<int>( ( args[vars[0]].second - args[vars[0]].first ) / eps );
			for( int i = 0; i < firstAxisSize; i++ ) {
				for( int j = 0; j < secondAxisSize; j++ ) {
					argToCount[vars[0]] = args[vars[0]].first + i * eps;
					argToCount[vars[1]] = args[vars[1]].first + j * eps;
					auto result = formula.Calculate( argToCount );
					points.push_back( C3DPoint( result['x'], result['y'], result['z'] ) );
					if( points.size() == 1 ) {
						maxZ = minZ = result['z'];
					} 
					maxZ = std::max( maxZ, result['z'] );
					minZ = std::min( minZ, result['z'] );
					if( j > 0 && i > 0 ) { 
						triangles.push_back( CTriangleIndex( points.size() - 1, points.size() - 2, points.size() - secondAxisSize - 1 ) );
						triangles.push_back( CTriangleIndex( points.size() - 2, points.size() - secondAxisSize - 2, points.size() - secondAxisSize - 1 ) );
					}
				}
			}
			
		} else {
			return false;
		}

	} catch( std::exception& ) {
		return false;
	}

	return true;
}