#include "SegmentIndex.h"

CSegmentIndex::CSegmentIndex(int first , int second):
First(first), Second(second)
{
	// Проверяем корректность переданных индексов
	if (first < 0 || second < 0 || first == second)
	{
		throw CSegmentIndex::IncorrectIndex();
	}
}