// Автор: Николай Левшин
// Назначение: Инициализация функций, использующихся для конвертации между форматами OpenMath и MathObj

#include "ConvertOM.h"
#include "tinyxml.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>


void LoadIdTable( IdCollection* ids )
{
    std::ifstream fin( "OMTable.txt" ); //  в данном файле содержится таблица соответствия атрибутов операндов идентификаторам
    std::string type, contDict, name;
    int id;
    while( fin ) // считываем файл и заполняем контейнер данными из таблицы
    {
        fin >> id >> type >> contDict >> name;
        if( fin )
        {
            if( name == "times" ) // так как умножения два, то пока что такой костыль.
            {
                if( contDict == "arith1" )
                {
                    ( *ids )[name + "1"] = std::make_pair( contDict, id );
                }
                else
                {
                    ( *ids )[name + "2"] = std::make_pair( contDict, id );
                }
            }
            else
            {
                ( *ids )[name] = std::make_pair( contDict, id );
            }
        }
    }
    fin.close( );
}
void LoadAttrTable( AttrCollection* attrs )
{
    std::ifstream fin( "OMTable.txt" ); //  в данном файле содержится таблица соответствия атрибутов операндов идентификаторам
    std::string type, contDict, name;
    int id;
    while( fin ) // считываем файл и заполняем контейнер данными из таблицы
    {
        fin >> id >> type >> contDict >> name;
        if( fin )
        {
            ( *attrs )[id] = std::make_pair( contDict, name );
        }
    }
    fin.close( );
}
void ConvertFromOM( std::string inputFileName, shared_ptr<MathObj> obj ) 
{
   
    IdCollection ids;
    LoadIdTable( &ids );

	TiXmlDocument doc;
	doc.LoadFile( inputFileName.data() );

	TiXmlHandle hDoc( &doc );
	TiXmlElement* pElem;

	pElem = hDoc.FirstChildElement().Element();

	ConvertElemToObj(&ids, pElem, obj );
}
void ConvertElemToObj( IdCollection* ids, TiXmlElement* pElem, shared_ptr<MathObj> obj ) {
	for( pElem; pElem; pElem = pElem->NextSiblingElement() ) //пробегаемся по всем элементам одного слоя
	{
		const std::string pKey = pElem->Value(); //получаем тэг текущей вершины
		const char *pText = pElem->GetText(); //получаем содержание тега, если оно есть, иначе NULL

		//Если корень
		if( pKey == "OMOBJ" ) 
		{
			ConvertElemToObj( ids, pElem->FirstChildElement(), obj );
		}

		//Если переменная
		else if( pKey == "OMV" ) 
		{
			TiXmlAttribute* pAttrib = pElem->FirstAttribute();
			shared_ptr<ParamObj> node ( new ParamObj( pAttrib->Value() ) );
            static_cast<FormulaObj*>( obj.get() )->params.push_back( node );
		}

		//Если целое число
		else if( pKey == "OMI" ) 
		{
			shared_ptr<ParamObj> node ( new ParamObj( pElem->GetText() ) );
			static_cast<FormulaObj*>(obj.get())->params.push_back( node );
		}

        //Если десятичное число
        else if( pKey == "OMF" )
        {
            TiXmlAttribute* pAttrib = pElem->FirstAttribute();
            shared_ptr<ParamObj> node ( new ParamObj( pAttrib->Value( ) ) );
			static_cast<FormulaObj*>(obj.get())->params.push_back( node );

        }
		//Если конструкция, содержащая массив операций
		else if( pKey == "OMA" ) 
		{
			shared_ptr<FormulaObj> node ( new FormulaObj() );
			static_pointer_cast<FormulaObj>(obj)->params.push_back(node);
			ConvertElemToObj( ids, pElem->FirstChildElement(), node );
		}

		//Если операнд
		else if( pKey == "OMS" ) 
		{
			TiXmlAttribute* pAttrib = pElem->FirstAttribute();
			std::string name = pAttrib->Name(); // Тип атрибута

            //Находим атрибут, отвечающий за название словаря, содержащего наш атрибут
            while( name != "cd" )
            {
                pAttrib = pAttrib->Next( );
                name = pAttrib->Name( );
            }

            std::string cd = pAttrib->Value(); // получаем требуемый словарь

			//Находим атрибут, отвечающий за тип операнда
			while( name != "name" ) 
			{
				pAttrib = pAttrib->Next();
				name = pAttrib->Name();
			}

			std::string operand = pAttrib->Value(); // Получаем тип операнда
            SetFormulaObjType( ids, static_pointer_cast<FormulaObj>( obj ), &cd, &operand );
		}
	}

}
void ConvertToOM( std::string outputFileName, shared_ptr<MathObj> obj ) 
{
	TiXmlDocument doc;
	TiXmlElement* element (new TiXmlElement( "OMOBJ" ));
	element->SetAttribute( "xmlns", "http://www.openmath.org/OpenMath" );
	doc.LinkEndChild( element );

    AttrCollection attrs;
    LoadAttrTable( &attrs );
    ConvertObjToElem( &attrs, element, static_pointer_cast<FormulaObj>( obj )->params[0] );

    doc.SaveFile( outputFileName.data() );
}
void ConvertObjToElem( AttrCollection* attrs, TiXmlElement* pElem, shared_ptr<MathObj> obj ) 
{
	std::string objType = typeid( *obj ).name();
	// std::cout << objType.c_str() << std::endl;
	if( objType == "class FormulaObj" ) 
    {
		TiXmlElement* element ( new TiXmlElement( "OMA" ) );
		pElem->LinkEndChild( element );

		TiXmlElement* formulaElement ( new TiXmlElement( "OMS" ) );
		 
		SetFormulaElementAttribute( attrs, formulaElement, ((FormulaObj*)obj.get())->GetType() );
		element->LinkEndChild(formulaElement);

		std::vector<shared_ptr<MathObj>>::iterator it = (dynamic_pointer_cast<FormulaObj>(obj)->params.begin());
		std::vector<shared_ptr<MathObj>>::const_iterator itEnd = (dynamic_pointer_cast<FormulaObj>(obj))->params.end();
		for( it; it != itEnd; ++it ) 
		{
			ConvertObjToElem( attrs, element, *it );
		}
    }
    else if( objType == "class ParamObj" )
    {
		std::string val = (static_pointer_cast< ParamObj >(obj))->GetVal();
        if( val[0] > 47 && val[0] < 58 ) // Если это число
        {
            if( val.find( '.' ) != std::string::npos ) // И в нем есть точка
            {
                TiXmlElement* element (new TiXmlElement( "OMF" ));
                element->SetAttribute( "dec", val.c_str() );
                pElem->LinkEndChild( element );
            }
            else
            {
                TiXmlElement* element = (new TiXmlElement( "OMI" ));
                TiXmlText* text = ( new TiXmlText( val.c_str() ) );
                element->LinkEndChild( text );
                pElem->LinkEndChild( element );
            }
        }
        else
        {
            TiXmlElement* element = ( new TiXmlElement( "OMV" ) );
            element->SetAttribute( "name", val.c_str( ) );
            pElem->LinkEndChild( element );
        }
    }
}
void SetFormulaElementAttribute( AttrCollection* attrs, TiXmlElement* pElem, TNodeType type ) 
{
    if( attrs->find( static_cast<int>( type ) ) != attrs->end() )
    {
        pElem->SetAttribute( "cd", (*attrs)[type].first.c_str() );
        pElem->SetAttribute( "name", (*attrs)[type].second.c_str() );
    }
    else
    {
		std::cerr << "Error: invalid type of Node in MathObj:" << type << std::endl;
	}
}
void SetFormulaObjType( IdCollection* ids, shared_ptr<FormulaObj> obj, std::string* cd, std::string* operand)
{
    if( *operand == "times" )// так как умножения два, то такой вот костыль, пару к нему ищи в функции LoadIdTable
    {
        if( *cd == "arith1" )
        {
            *operand += "1";
        }
        else
        {
            *operand += "2";
        }
    }

    obj->SetType( TNodeType( (*ids)[*operand].second ) );
    //std::cout << *operand << " " << (*ids)[*operand].second << std::endl;
}

