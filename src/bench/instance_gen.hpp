/**===============================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Optimal Instance generator
 *================================================**/

#include <iostream>
#include <random>

#include "../types.h"

std::random_device seeder;
std::mt19937 engine(seeder());

// ( better than rand() )
int randrange(int min, int max) // endpoints included
{
	return std::uniform_int_distribution<int>(min, max)(engine);
}

// Generate Random SPP Instance
SHAPE_VEC* genInstance(int W, int N, float RATIO)
{
	SHAPE_SET* rectangles = new SHAPE_SET;
	rectangles->insert(new SHAPE(1, 0, 0, W, int(float(W)*RATIO)));

	int n = 1;
	bool horizontal = true;

	// while there aren't N rectangles inside the instance...
	while (n != N)
	{
		SHAPE_SET* split = new SHAPE_SET; // container for split rectangles
		for (SHAPE* rectangle : *rectangles)
		{
			if (n == N || (horizontal && rectangle->w == 1) || (!horizontal && rectangle->h == 1))
			{
				split->insert(rectangle);
				continue;
			}

			if (horizontal)
			{
				int cut = randrange(1, rectangle->w-1);
				split->insert(new SHAPE(rectangle->id, rectangle->x, rectangle->y, cut, rectangle->h));
				n++;
				split->insert(new SHAPE(n, rectangle->x+cut, rectangle->y, rectangle->w-cut, rectangle->h));
			}
			else
			{
				int cut = randrange(1, rectangle->h-1);
				split->insert(new SHAPE(rectangle->id, rectangle->x, rectangle->y, rectangle->w, cut));
				n++;
				split->insert(new SHAPE(n, rectangle->x, rectangle->y+cut, rectangle->w, rectangle->h-cut));
			}
		}

		delete rectangles;
		rectangles = split;

		horizontal = !horizontal;
	}

	SHAPE_VEC* boxList = new SHAPE_VEC(rectangles->begin(), rectangles->end());
	delete rectangles;

	return boxList;
}
