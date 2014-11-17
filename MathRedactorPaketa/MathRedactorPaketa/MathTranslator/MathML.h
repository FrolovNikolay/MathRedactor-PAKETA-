#include "stdafx.h"
#include "MathFormObj.h"
#include "targetver.h"
#include "tinystr.h"
#include "tinyxml.h"
#include <string>
#include <iostream>
#include <stack>
#include <memory>

using namespace std;

class MathMLParser
{
private:
	shared_ptr<MathObj> root;
public:
	void SetData(shared_ptr<MathObj> newRoot){ root = newRoot; }
	shared_ptr<MathObj> GetData(){ return root; }
	void Pars(const std::string file);
    void Save(std::string file);
};

class CTreeBuilder
{
	stack< TiXmlElement* > elements;
	stack<TNodeType> operations;
	stack< shared_ptr<MathObj> > terms;
	bool prevIsTerm = false;
	bool uminus = false;
public:
	bool IsTree(){ return terms.top() != 0; }
	shared_ptr<MathObj> GetObj( );
	TiXmlElement* GetArg(){ return elements.top(); }
	void Push( TiXmlElement* elem );
};

int priority( TNodeType a );

bool compare( TNodeType a, TNodeType b );

TNodeType readBinarOperation( TiXmlElement* elem );

void addArgToData( TiXmlElement* elem, vector<shared_ptr<MathObj>>::iterator place );

void addRowToData( TiXmlElement* elem, vector<shared_ptr<MathObj>>::iterator place );

void saveTreeToXml( TiXmlElement*, shared_ptr<MathObj>, bool ); // преобразует дерево разбора в xml документ, начиная с определенной вершины

void linkNewElem( TiXmlElement*, shared_ptr<char>, shared_ptr<std::string> ); // добавляет в xml документе новый элемент с заданным тегом и значением