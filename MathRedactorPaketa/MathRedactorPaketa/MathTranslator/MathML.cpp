#include"MathML.h"

void CTreeBuilder::Push( TiXmlElement* elem )
{
	if( uminus )
	{
		if( elem->Value() != string( "mo" ) )
		{
			shared_ptr<FormulaObj> temp ( new FormulaObj() );
			temp->SetType( NT_UMINUS );
			temp->params.push_back( NULL );
			addArgToData( elem, temp->params.begin() );
			terms.push( temp );
			prevIsTerm = true;
			uminus = false;
		}
		else
		{
			cout << "error reading unary minus!" << endl;
		}
	}
	else
	{
		bool invisibleMult = false;
		TNodeType operation;
		if( elem->Value() == string( "mo" ) )
		{
			if( prevIsTerm )
			{
				operation = readBinarOperation( elem );
				prevIsTerm = false;
			}
			else
			{
				if( elem->GetText() == string( "-" ) )
				{
					uminus = true;
				}
				else
				{
					cout << "error reading binary operation!" << endl;
				}
				return;
			}
		}
		else
		{
			if( prevIsTerm )
			{
				operation = NT_MULTCM;
				invisibleMult = true;
			}
			else
			{
				terms.push( NULL );
				elements.push( elem );
				prevIsTerm = true;
				return;
			}
		}
		while( operations.size() && !compare( operation, operations.top() ) )
		{
			shared_ptr<FormulaObj> temp ( new FormulaObj() );
			temp->SetType( operations.top() );
			operations.pop();
			shared_ptr<MathObj> secondArg = terms.top();
			terms.pop();
			shared_ptr<MathObj> firstArg = terms.top();
			terms.pop();
			temp->params.push_back( firstArg );
			temp->params.push_back( secondArg );
			if( secondArg == NULL || firstArg == NULL )
			{
				vector<shared_ptr<MathObj>>::iterator argPlace = temp->params.begin();
				++argPlace;
				if( secondArg == NULL )
				{
					addArgToData( elements.top(), argPlace );
					elements.pop();
				}
				--argPlace;
				if( firstArg == NULL )
				{
					addArgToData( elements.top(), argPlace );
					elements.pop();
				}
			}
			terms.push( temp );
		}
		operations.push(operation);
		if( invisibleMult )
		{
			terms.push( NULL );
			elements.push( elem );
		}
	}
}

shared_ptr<MathObj> CTreeBuilder::GetObj()
{
	while( operations.size( ) )
	{
		shared_ptr<FormulaObj> temp (new FormulaObj( ));
		temp->SetType( operations.top( ) );
		operations.pop( );
		shared_ptr<MathObj> secondArg = terms.top( );
		terms.pop( );
		shared_ptr<MathObj> firstArg = terms.top( );
		terms.pop( );
		temp->params.push_back( firstArg );
		temp->params.push_back( secondArg );
		if( secondArg == NULL || firstArg == NULL )
		{
			vector<shared_ptr<MathObj>>::iterator argPlace = temp->params.begin( );
			++argPlace;
			if( secondArg == NULL )
			{
				addArgToData( elements.top( ), argPlace );
				elements.pop( );
			}
			--argPlace;
			if( firstArg == NULL )
			{
				addArgToData( elements.top( ), argPlace );
				elements.pop( );
			}
		}
		terms.push( temp );
	}
	return terms.top();
}

int priority( TNodeType a )
{
	switch( a )
	{
	case NT_PLUS:
		return 2;
		break;
	case NT_UMINUS:
		return 3;
		break;
	case NT_MINUS:
		return 2;
		break;
	case NT_MULTNCM:
		return 3;
		break;
	case NT_DIV:
		return 3;
		break;
	case NT_POW:
		return 4;
		break;
	case NT_ABS:
		return 4;
		break;
	case NT_ROOT:
		return 4;
		break;
	case NT_SUM:
		return 4;
		break;
	case NT_PROD:
		return 4;
		break;
	case NT_MULTCM:
		return 4;
		break;
	case NT_INVERSE:
		return 4;
		break;
	case NT_LOG:
		return 4;
		break;
	case NT_LN:
		return 4;
		break;
	case NT_EXP:
		return 4;
		break;
	case NT_SIN:
		return 4;
		break;
	case NT_COS:
		return 4;
		break;
	case NT_TAN:
		return 4;
		break;
	case NT_SEC:
		return 4;
		break;
	case NT_CSC:
		return 4;
		break;
	case NT_COT:
		return 4;
		break;
	case NT_SINH:
		return 4;
		break;
	case NT_COSH:
		return 4;
		break;
	case NT_TANH:
		return 4;
		break;
	case NT_SECH:
		return 4;
		break;
	case NT_CSCH:
		return 4;
		break;
	case NT_COTH:
		return 4;
		break;
	case NT_ARCSIN:
		return 4;
		break;
	case NT_ARCCOS:
		return 4;
		break;
	case NT_ARCTAN:
		return 4;
		break;
	case NT_ARCSEC:
		return 4;
		break;
	case NT_ARCCSC:
		return 4;
		break;
	case NT_ARCCOT:
		return 4;
		break;
	case NT_ARCSINH:
		return 4;
		break;
	case NT_ARCCOSH:
		return 4;
		break;
	case NT_ARCTANH:
		return 4;
		break;
	case NT_ARCSECH:
		return 4;
		break;
	case NT_ARCCSCH:
		return 4;
		break;
	case NT_ARCCOTH:
		return 4;
		break;
	case NT_EQUAL:
		return 1;
		break;
	case NT_LESS:
		return 1;
		break;
	case NT_GREAT:
		return 1;
		break;
	case NT_NEQUAL:
		return 1;
		break;
	case NT_LESSEQ:
		return 1;
		break;
	case NT_GREATEQ:
		return 1;
		break;
	case NT_APPROX:
		return 1;
		break;
	case NT_PLUSMINUS:
		return 2;
		break;
	case NT_MAIN:
		return 0;
		break;
	case NT_NOTYPE:
		return 0;
		break;
	default:
		return 0;
		break;
	}
}

bool compare( TNodeType a, TNodeType b )
{
	return priority( a ) > priority( b );
}

TNodeType readBinarOperation( TiXmlElement* elem )
{
	string id( elem->GetText( ) );
	if( id == "+" )
	{
		return NT_PLUS;
	}
	if( id == "-" )
	{
		return NT_MINUS;
	}
	if( id == "*" )
	{
		return NT_MULTCM;
	}
	if( ( int ) id[0] == ( int )'±' )
	{
		return NT_PLUSMINUS;
	}
	if( id == "=" )
	{
		return NT_EQUAL;
	}
	if( id == "<" )
	{
		return NT_LESS;
	}
	if( id == ">" )
	{
		return NT_GREAT;
	}
	if( id == "≈" )
	{
		return NT_APPROX;
	}
	if( id == "\leqslant" || id == "\leq" )
	{
		return NT_LESSEQ;
	}
	if( id == "\geqslant" || id == "\geq" )
	{
		return NT_GREATEQ;
	}
	if( id == "≠" )
	{
		return NT_NEQUAL;
	}
	return NT_NOTYPE;
}

void MathMLParser::Pars(const std::string file)
{ 
	TiXmlDocument doc(file.data());
	bool loadOkay = doc.LoadFile();
	if (!loadOkay)
	{
		cout << "Failed to load MML-file\n";
	}
	TiXmlElement* elem (doc.FirstChildElement());
	elem = (elem->FirstChildElement());
	root = shared_ptr<FormulaObj>(new FormulaObj());
	((FormulaObj*)root.get())->SetType(NT_MAIN);
	((FormulaObj*)root.get())->params.push_back(NULL);
	vector<shared_ptr<MathObj>>::iterator place = ((FormulaObj*)root.get())->params.begin();
	addArgToData(elem, place);
}

void addArgToData( TiXmlElement* elem, vector<shared_ptr<MathObj>>::iterator place )
{
	if( elem->Value( ) == string( "msqrt" ) )
	{
		shared_ptr<FormulaObj> child (new FormulaObj( ));
		*place = child;
		child->SetType( NT_ROOT );
		child->params.push_back( NULL );
		vector<shared_ptr<MathObj>>::iterator argPlace = child->params.begin( );
		elem = elem->FirstChildElement();
		addArgToData( elem, argPlace );
		return;
	}

	if( elem->Value( ) == string( "msup" ) )
	{
		shared_ptr<FormulaObj> child (new FormulaObj( ));
		*place = child;
		child->SetType( NT_POW );
		child->params.push_back( NULL );
		child->params.push_back( NULL );
		vector<shared_ptr<MathObj>>::iterator argPlace = child->params.begin( );
		elem = elem->FirstChildElement();
		addArgToData( elem, argPlace );
		++argPlace;
		elem = elem->NextSiblingElement( );
		addArgToData( elem, argPlace );
		return;
	}
	if( elem->Value( ) == string( "mfrac" ) )
	{
		shared_ptr<FormulaObj> child (new FormulaObj( ));
		*place = child;
		child->SetType( NT_DIV );
		child->params.push_back( NULL );
		child->params.push_back( NULL );
		vector<shared_ptr<MathObj>>::iterator argPlace = child->params.begin( );
		elem = elem->FirstChildElement( );
		addArgToData( elem, argPlace );
		++argPlace;
		elem = elem->NextSiblingElement();
		addArgToData( elem, argPlace );
		return;
	}
	if( elem->Value() == string( "mi" ) || elem->Value() == string( "mn" ) )
	{
		shared_ptr<ParamObj> arg (new ParamObj( ));
		*place = arg;
		arg->SetVal( elem->GetText() );
		return;
	}
	if( elem->Value( ) == string( "mrow" ) )
	{
		TiXmlElement* childElem(elem->FirstChildElement( ));
		addRowToData( childElem, place );
		return;
	}
	if( elem->Value() == string( "mfenced" ) )
	{
		TiXmlElement* childElem (elem->FirstChildElement( ));
		if( elem->Attribute( "open" ) == string( "|" ) && elem->Attribute( "close" ) == string( "|" ) )
		{
			shared_ptr<FormulaObj> child (new FormulaObj( ));
			*place = child;
			child->SetType( NT_ABS );
			child->params.push_back( NULL );
			addRowToData( childElem, child->params.begin( ) );
		}
		else
		{
			addRowToData( childElem, place );
		}
		return;
	}
}

void addRowToData( TiXmlElement* elem, vector<shared_ptr<MathObj>>::iterator place )
{
	CTreeBuilder rowTree;
	for( elem; elem; elem = elem->NextSiblingElement() )
	{
		if( elem->Value( ) == string( "mrow" ) )
		{
			TiXmlElement* subElem = elem->FirstChildElement();
			for( subElem; subElem; subElem = subElem->NextSiblingElement() )
			{
				rowTree.Push( subElem );
			}
		}
		else
		{
			rowTree.Push( elem );
		}
	}
	*place = rowTree.GetObj( );
	if( *place == NULL )
	{
		addArgToData( rowTree.GetArg( ), place );
	}
}


void MathMLParser::Save(std::string outputFileName )
{
    TiXmlDocument doc;
    TiXmlDeclaration* decl ( new TiXmlDeclaration( "1.0", "UTF-8", "" ) );
    doc.LinkEndChild( decl ); //создали декларационную часть xml документа
    
    shared_ptr<MathObj> obj = root;
    if( static_cast< FormulaObj* >( obj.get() )->GetType() != NT_MAIN )
    {
        std::cerr << "Ошибка построения дерева разбора: Корень должен быть NT_MAIN!" << std::endl;
        return;
    }
    
	TiXmlElement* element (new TiXmlElement( "math" ));
    element->SetAttribute( "xmlns", "http://www.w3.org/1998/Math/MathML" );
    doc.LinkEndChild( element ); //создали главную часть, в которой записана формула
    saveTreeToXml( element, static_cast< FormulaObj* >( obj.get() )->params[0], true );

    doc.SaveFile( outputFileName.data() );
}

void linkNewElem( TiXmlElement* parent, const char* param, const char* val )
{
    TiXmlElement* element (new TiXmlElement( param ));
    TiXmlText* text ( new TiXmlText( val ) );
    element->LinkEndChild( text );
    parent->LinkEndChild( element );
}

void saveTreeToXml( TiXmlElement* pElem, shared_ptr<MathObj> obj, bool isNewRow )
{
    if( isNewRow ) {
        TiXmlElement* element (new TiXmlElement( "mrow" ));
        pElem->LinkEndChild( element );
        pElem = element;
        isNewRow = false;
    }

    std::string objType = typeid( *obj ).name();

    if( objType == "class ParamObj" ) 
    {
        std::string val = static_pointer_cast< ParamObj >( obj )->GetVal();
        if( val[0] < 48 || val[0] > 57 ) // если значение начинается с буквы, то - идентификатор 
        {
            linkNewElem( pElem, "mi", val.c_str() );
        }
        else {
            linkNewElem( pElem, "mn", val.c_str() );
        }
    }
    else if( objType == "class FormulaObj" )
    {
        shared_ptr<FormulaObj> node = static_pointer_cast< FormulaObj >( obj );
        TNodeType type = node->GetType();
        std::vector<shared_ptr<MathObj>>::iterator it = node->params.begin( );
        std::vector<shared_ptr<MathObj>>::const_iterator itLast = node->params.end( ) - 1;
        TiXmlElement* element;

        switch( type )
        {
        case NT_PLUS:
            for( it; it != itLast; ++it )
            {
                saveTreeToXml( pElem, *it, isNewRow );
                linkNewElem( pElem, "mo", "+" );
            }
            saveTreeToXml( pElem, *it, isNewRow );
            break;

        case NT_UMINUS:
            linkNewElem( pElem, "mo", "-" );
            saveTreeToXml( pElem, *it, isNewRow );
            break;
            
        case NT_MINUS:
            saveTreeToXml( pElem, *it, isNewRow );
            linkNewElem( pElem, "mo", "-" );
            saveTreeToXml( pElem, *itLast, isNewRow );
            break;

        case NT_MULTNCM: // TODO поправить в будущем, пока как обычное умножение
            for( it; it != itLast; ++it )
            {
                saveTreeToXml( pElem, *it, isNewRow );
            }
            saveTreeToXml( pElem, *it, isNewRow );
            break;

        case NT_DIV:
            element = (new TiXmlElement( "mfrac" ));
            pElem->LinkEndChild( element );
            saveTreeToXml( element, *it, true );
            saveTreeToXml( element, *itLast, true );
            break;

        case NT_POW:
            element = (new TiXmlElement( "msup" ));
            pElem->LinkEndChild( element );
            saveTreeToXml( element, *it, false );
            saveTreeToXml( element, *itLast, false );
            break;

        case NT_ROOT:
            if( itLast == it )
            {
				element = (new TiXmlElement("msqrt"));
                pElem->LinkEndChild( element );
                saveTreeToXml( element, *it, false );
            }
            break;

        case NT_MULTCM:
            for( it; it != itLast; ++it )
            {
                saveTreeToXml( pElem, *it, isNewRow );
            }
            saveTreeToXml( pElem, *it, isNewRow );
            break;

        case NT_EQUAL:
            saveTreeToXml( pElem, *it, isNewRow );
            linkNewElem( pElem, "mo", "=" );
            saveTreeToXml( pElem, *itLast, isNewRow );
            break;

        case NT_LESS:
            saveTreeToXml( pElem, *it, isNewRow );
            linkNewElem( pElem, "mo", "<" );
            saveTreeToXml( pElem, *itLast, isNewRow );
            break;

        case NT_GREAT:
            saveTreeToXml( pElem, *it, isNewRow );
            linkNewElem( pElem, "mo", ">" );
            saveTreeToXml( pElem, *itLast, isNewRow );
            break;

        case NT_NEQUAL:
            saveTreeToXml( pElem, *it, isNewRow );
            linkNewElem( pElem, "mo", "≠" );
            saveTreeToXml( pElem, *itLast, isNewRow );
            break;

        case NT_LESSEQ:
            saveTreeToXml( pElem, *it, isNewRow );
            linkNewElem( pElem, "mo", "\leq" );
            saveTreeToXml( pElem, *itLast, isNewRow );
            break;

        case NT_GREATEQ:
            saveTreeToXml( pElem, *it, isNewRow );
            linkNewElem( pElem, "mo", "\geq" );
            saveTreeToXml( pElem, *itLast, isNewRow );
            break;

        case NT_APPROX:
            saveTreeToXml( pElem, *it, isNewRow );
            linkNewElem( pElem, "mo", "≈" );
            saveTreeToXml( pElem, *itLast, isNewRow );
            break;

        case NT_PLUSMINUS:
            saveTreeToXml( pElem, *it, isNewRow );
            linkNewElem( pElem, "mo", "±" );
            saveTreeToXml( pElem, *itLast, isNewRow );
            break;

        default:
            std::cerr << "Ошибка, не поддерживаемый (пока) тип оператора, код: " << type << std::endl;
        }
    }
}