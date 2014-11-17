#include "TriangleIndex.h"


CTriangleIndex::CTriangleIndex(int first, int second, int third) :
First(first), Second(second), Third(third)
{
	// Проверяем корректность индекса
	if (first < 0 || second < 0 || third < 0 || first == second || first == third || second == third)
	{
		throw CTriangleIndex::IncorrectIndex();
	}
}
