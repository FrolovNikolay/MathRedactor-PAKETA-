// Автор: Азат Давлетшин
// Описание: реализация конвертера Latex <-> MathObj*

#include "ConvertLatex.h"
#include <assert.h>
#include <iostream>

using namespace std;

shared_ptr<MathObj> CConvertLatex::ConvertFromLatex( const string& input )
{
	cout << input;
	parser.Init();

	// Стартовое состояние лексера - TT_SPACE, т.к. оно игнорируется парсером и принимает любые символы
	shared_ptr<CLatexState> state (new CLatexSingleState( TT_SPACE ));

	for( int i = 0; i < input.length( ); ++i ) {
		shared_ptr<CLatexState> nextState = state->Accept( input[i] );
		parser.TryParse( state );
		state = nextState;
	}

	// Символы в строке закончились, поэтому нужно отсоединить последний токен и протолкнуть в парсер
	state->Detach();
	parser.TryParse( state );

	shared_ptr<MathObj> root = parser.End();
	return root;
}

void CConvertLatex::ConvertToLatex( shared_ptr<MathObj> moTree, string& output ) const
{
	FormulaObj* fobj = dynamic_cast<FormulaObj*>( moTree.get() );
	assert( fobj != 0 );
	assert( fobj->GetType() == NT_MAIN );
	writeNode( fobj->params[0], output );
}

void CConvertLatex::writeNode( shared_ptr<MathObj> node, string& output ) const
{
	shared_ptr<FormulaObj> fobj = dynamic_pointer_cast<FormulaObj>( node );
	if( fobj == 0 ) {
		// ParamObj, выводим ID
		shared_ptr<ParamObj> pobj = dynamic_pointer_cast<ParamObj>( node );
		assert( pobj != 0 );
		output += pobj->GetVal();
		return;
	}

	switch( fobj->GetType() )  
	{
		case NT_MULTCM:
			return writeBinop( fobj, output );
		case NT_PLUS:
			return writeBinop( fobj, output );
		case NT_MINUS:
			return writeBinop( fobj, output );
		case NT_DIV:
			return writeDiv( fobj, output );
		case NT_POW:
			return writePow( fobj, output );
		case NT_ROOT:
			return writeRoot( fobj, output );
		case NT_SUM:
			return writeSumOrProd( fobj, output );
		case NT_PROD:
			return writeSumOrProd( fobj, output );
		case NT_SIN:
			return writeTrigon( fobj, output );
		case NT_COS:
			return writeTrigon( fobj, output );
		case NT_TAN:
			return writeTrigon( fobj, output );
		case NT_COT:
			return writeTrigon( fobj, output );
		case NT_UMINUS:
			return writeUMinus( fobj, output );
		default:
			assert( false );
	}
}

int CConvertLatex::priority( shared_ptr<MathObj> node )
{
	FormulaObj* fobj = dynamic_cast<FormulaObj*>( node.get() );
	if( fobj == 0 ) {
		// ID
		return INT_MAX;
	}

	switch( fobj->GetType() )  
	{
		case NT_PLUS:
			return 0;
		case NT_MINUS:
			return 0;
		case NT_SUM:
			return 1;
		case NT_PROD:
			return 1;
		case NT_SIN:
			return 2;
		case NT_COS:
			return 2;
		case NT_TAN:
			return 2;
		case NT_COT:
			return 2;
		case NT_MULTCM:
			return 3;
		case NT_DIV:
			return 3;
		case NT_UMINUS:
			return 4;
		case NT_POW:
			return 5;
		case NT_ROOT:
			return INT_MAX;
		default:
			assert( false );
	}

	return 0;
}

void CConvertLatex::writeChild( shared_ptr<MathObj> parent, shared_ptr<MathObj> child, string& output ) const
{
	if( priority( child ) < priority( parent ) ) {
		output += '(';
		writeNode( child, output );
		output += ')';
	} else {
		writeNode( child, output );
	}
}

void CConvertLatex::writeBinop( shared_ptr<FormulaObj> node, string& output ) const
{
	writeChild( node, node->params[0], output );

	switch( node->GetType() )  
	{
		case NT_MULTCM:
			output += '*';
			break;
		case NT_PLUS:
			output += '+';
			break;
		case NT_MINUS:
			output += '-';
			break;
		default:
			assert( false );
	}

	writeChild( node, node->params[1], output );
}

void CConvertLatex::writeDiv( shared_ptr<FormulaObj> node, string& output ) const
{
	assert( node->GetType() == NT_DIV );

	output += "\\frac{";
	writeNode( node->params[0], output );
	output += "}{";
	writeNode( node->params[1], output );
	output += '}';
}

void CConvertLatex::writePow( shared_ptr<FormulaObj> node, string& output ) const
{
	assert( node->GetType() == NT_POW );

	writeChild( node, node->params[0], output );

	output += "^{";
	writeNode( node->params[1], output );
	output += '}';
}

void CConvertLatex::writeSumOrProd( shared_ptr<FormulaObj> node, string& output ) const
{
	switch( node->GetType() )  
	{
		case NT_SUM:
			output += "\\sum_{";
			break;
		case NT_PROD:
			output += "\\prod_{";
			break;
		default:
			assert( false );
	}

	writeNode( node->params[0], output );
	output += "}^{";
	writeNode( node->params[1], output );
	output += "}(";
	writeNode( node->params[2], output );
	output += ")";
}

void CConvertLatex::writeTrigon( shared_ptr<FormulaObj> node, string& output ) const
{
	switch( node->GetType() )  
	{
		case NT_SIN:
			output += "\\sin(";
			break;
		case NT_COS:
			output += "\\cos(";
			break;
		case NT_TAN:
			output += "\\tan(";
			break;
		case NT_COT:
			output += "\\cot(";
			break;
		default:
			assert( false );
	}

	writeNode( node->params[0], output );
	output += ')';
}

void CConvertLatex::writeUMinus( shared_ptr<FormulaObj> node, string& output ) const
{
	assert( node->GetType() == NT_UMINUS );

	output += '-';

	writeChild( node, node->params[0], output );
}

void CConvertLatex::writeRoot(shared_ptr<FormulaObj> node, string& output) const  
{
	assert(node->GetType() == NT_ROOT);

	output += "\\sqrt{";
	writeNode(node->params[0], output);
	output += "}";
}
