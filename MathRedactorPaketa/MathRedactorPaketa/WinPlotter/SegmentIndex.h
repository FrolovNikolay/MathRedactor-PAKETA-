#pragma once

// —труктура, хран¤ща¤ индексы точек начала и конца отрезка
struct CSegmentIndex
{
	int First, Second;
	CSegmentIndex(int first, int second);

	// »сключение, возникающее, когда мы хотим создать отрезок с одинаковыми или отрицательными индексами
	class IncorrectIndex
	{
	};
};

