#include <iostream>
#include "CRTTriangle.h"
#include "CRTVector.h"

int main()
{
	// Task 1
	CRTTriangle triangle1(CRTVector(1, 2, 3), CRTVector(1, 2, 3), CRTVector(1, 2, 3));
	CRTTriangle triangle2 = triangle1;
	CRTTriangle triangle3({
		CRTVector(3, 2, 1), 
		CRTVector(3, 2, 1), 
		CRTVector(3, 2, 1)
		});

	// Task 2
	CRTVector subTask1 = cross(CRTVector(3.5, 0, 0), CRTVector(1.75, 3.5, 0));
	CRTVector subTask2 = cross(CRTVector(3, -3, 1), CRTVector(4, 9, 3));

	float subTask3 = abs(scalar(CRTVector(3, -3, 1), CRTVector(4, 9, 3)));
	float subTask4= abs(scalar(CRTVector(3, -3, 1), CRTVector(-12, 12, -4)));

	// Task 3
	CRTTriangle t1({
		CRTVector(-1.75, -1.75, -3),
		CRTVector(1.75, -1.75, -3),
		CRTVector(0, 1.75, -3) 
		});
	CRTVector normalT1 = t1.normalVector();

	CRTTriangle t2({
		CRTVector(0, 0, -1),
		CRTVector(1, 0, 1),
		CRTVector(-1, 0, 1) 
		});
	CRTVector normalT2 = t2.normalVector();

	CRTTriangle t3({
		CRTVector(0.56, 1.11, 1.23),
		CRTVector(0.44, -2.368, -0.54),
		CRTVector(-1.56, 0.15, -1.92)
		});
	CRTVector normalT3 = t3.normalVector();

	float areaT1 = t1.area();
	float areaT2 = t2.area();
	float areaT3 = t3.area();
}
