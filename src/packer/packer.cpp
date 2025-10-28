/**==============================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Main packer algorithm
 *=============================================**/

#include <iostream>
#include <algorithm>
#include <chrono>
#include <math.h>

#include "packer.h"

#define CHECK_VALID 0
#define INT_INFINITY 100000000

int nbHole = 0;

// -------------------------------
// ~ SHAPE Class
// -------------------------------

// Get Bottom Right Coordinates
int SHAPE::getx2()
{
	return this->x + this->w;
}
int SHAPE::gety2()
{
	return this->y + this->h;
}

// Get Area
int SHAPE::getArea()
{
	if (!this->a) // Store area -> prevents repeating calculations
		this->a = this->w*this->h;
	return this->a;
}

// Rotating a shape = Swapping the dimensions
void SHAPE::rotate()
{
	std::swap(this->w, this->h);
	this->isRotated = !this->isRotated;
}

// Compare two SHAPES based on basic properties (x,y,w,h)
bool SHAPE::operator==(SHAPE& shape)
{
	return (
		this->x == shape.x &&
		this->y == shape.y &&
		this->w == shape.w &&
		this->h == shape.h
	);
}

// Check if a shape fits in another using
// a comparison of dimensions (theoretical)
bool SHAPE::fitsIn(SHAPE*& shape)
{
	return (
		this->w <= shape->w &&
		this->h <= shape->h
	);
};

// Check if a shape fits in another using
// a comparison of positions (applied)
bool SHAPE::isIn(SHAPE*& shape)
{
	return (
		this->x >= shape->x &&              // TL corner of 'this' shape is inside 'shape'
		this->y >= shape->y &&              //

		this->getx2() <= shape->getx2() &&  // BR corner of 'this' shape is inside 'shape'
		this->gety2() <= shape->gety2()     //
	);
}

// Collision detection between 2 shapes
// (Sharing a common border is not considered an intersect)

// Detects overlap between two intervals A and B
bool overlaps(int Amin, int Amax, int Bmin, int Bmax)
{
	return Amin < Bmax && Amax > Bmin;
}

bool SHAPE::intersects(SHAPE*& shape)
{
	return ( 
		overlaps(this->x, this->getx2(), shape->x, shape->getx2()) &&   // Two shapes only overlap
		overlaps(this->y, this->gety2(), shape->y, shape->gety2())      // if they overlap in all dimensions
	);
}

bool SHAPE::isCovered(SHAPE_SET*& shapes)
{
	// Iterates through all the shapes and checks if it is covered by a Shape AND
	for (SHAPE_SET::iterator it = shapes->begin(); it != shapes->end(); it++)
	{
		SHAPE* shape = *it;
		if (this->isIn(shape))
			return true;
	}
	return false;
}

// -------------------------------
// ~ COMPARATORS
// ? Returns true if a is before b
// -------------------------------
bool ascendingId(SHAPE*& a, SHAPE*& b) // Ascending Id
{
	return (a->id < b->id);
}
bool descendingArea(SHAPE*& a, SHAPE*& b) // Descending Area
{
	if (a->getArea() > b->getArea()) return true;
	if (a->getArea() != b->getArea()) return false;

	if (a->w > b->w) return true;
	if (a->w != b->w) return false;

	if (a->h > b->h) return true;
	if (a->h != b->h) return false;

	if (a->id < b->id) return true;

	return false;
}
bool descendingArea2(SHAPE*& a, SHAPE*& b) // Descending Area
{
	if ((a->getArea() + a->h) > (b->getArea() + b->h)) return true;
	if ((a->getArea()+ a->h) != (b->getArea() + b->h)) return false;

	if (a->w > b->w) return true;
	if (a->w != b->w) return false;

	if (a->h > b->h) return true;
	if (a->h != b->h) return false;

	if (a->id < b->id) return true;

	return false;
}
bool ascendingArea(SHAPE*& a, SHAPE*& b)
{
	return descendingArea(b, a); // Inverse arguments
}

bool descendingWidth(SHAPE*& a, SHAPE*& b) // Descending Width
{
	if (a->w > b->w) return true;
	if (a->w != b->w) return false;

	if (a->h > b->h) return true;
	if (a->h != b->h) return false;

	if (a->getArea() > b->getArea()) return true;
	if (a->getArea() != b->getArea()) return false;

	if (a->id < b->id) return true;

	return false;
}
bool ascendingWidth(SHAPE*& a, SHAPE*& b)
{
	return descendingWidth(b, a); // Inverse arguments
}

bool descendingHeight(SHAPE*& a, SHAPE*& b) // Descending Height
{
	if (a->h > b->h) return true;
	if (a->h != b->h) return false;

	if (a->w > b->w) return true;
	if (a->w != b->w) return false;

	if (a->getArea() > b->getArea()) return true;
	if (a->getArea() != b->getArea()) return false;

	if (a->id < b->id) return true;

	return false;
}
bool ascendingHeight(SHAPE*& a, SHAPE*& b)
{
	return descendingHeight(b, a); // Inverse arguments
}

// -------------------------------
// ~ PACKER
// -------------------------------

// cutHole : Creates/Cuts Hole into new Holes based on a Shape
void cutHole(SHAPE*& rectangle, SHAPE*& hole, SHAPE_SET*& holes)
{
	// Create new holes from a Placement / Overlapping
	// ⬛ => Hole
	// ⬜ => Rectangle
	// So we enumerate all the 16 cases where the rectangle can clip inside the Hole

	int rx2 = rectangle->getx2();
	int ry2 = rectangle->gety2();
	int hx2 = hole->getx2();
	int hy2 = hole->gety2();

    SHAPE_VEC newHoles;

	// ⬜⬜⬜
	// ⬜⬜⬜
	// ⬜⬜⬜
	// [CASE 1]: Perfect fit!
	if (
		rectangle == hole
	) {} // Only case where we add 0 Holes
	//

	// ⬜⬜⬜
	// ⬛⬛⬛
	// ⬛⬛⬛
	// [CASE 2]: Top Bar Horizontal
	else if (
		rectangle->x <= hole->x &&  // Rectangle's Left Vertical Edge is left to the hole AND
		rectangle->y <= hole->y &&  // Rectangle's Top Horizontal Edge is above the hole AND

		rx2 >= hx2 &&               // Rectangle's Right Vertical Edge is right to the hole AND

		ry2 > hole->y &&            // Rectangle's Bottom Horizontal Edge passes through the hole
		ry2 < hy2                   //
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, ry2, hx2-hole->x, hy2-ry2));
	}
	//

	// ⬛⬛⬜
	// ⬛⬛⬜
	// ⬛⬛⬜
	// [CASE 3]: Right Bar Vertical
	else if (
		rectangle->x > hole->x &&   // Rectangle's Left Vertical Edge passes through the hole AND
		rectangle->x < hx2 &&       // 

		rectangle->y <= hole->y &&  // Rectangle's Top Horizontal Edge is above the hole AND

		rx2 >= hx2 &&               // Rectangle's Right Vertical Edge is right to the hole AND

		ry2 >= hy2                  // Rectangle's Bottom Horizontal Edge is below the hole
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, rectangle->x-hole->x, hy2-hole->y));
	}
	//

	// ⬛⬛⬛
	// ⬛⬛⬛
	// ⬜⬜⬜
	// [CASE 4]: Bottom Bar Horizontal
	else if (
		rectangle->x <= hole->x &&  // Rectangle's Left Vertical Edge is left to the hole AND

		rectangle->y > hole->y &&   // Rectangle's Top Horizontal Edge passes through the hole AND
		rectangle->y < hy2 &&       //

		rx2 >= hx2 &&               // Rectangle's Right Vertical Edge is right to the hole AND

		ry2 >= hy2                  // Rectangle's Bottom Horizontal Edge is below the hole
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, hx2-hole->x, rectangle->y-hole->y));
	}
	//

	// ⬜⬛⬛
	// ⬜⬛⬛
	// ⬜⬛⬛
	// [CASE 5]: Left Bar Vertical
	else if (
		rectangle->x <= hole->x &&  // Rectangle's Left Vertical Edge is left to the hole AND

		rectangle->y <= hole->y &&  // Rectangle's Top Horizontal Edge is above the hole AND

		rx2 > hole->x &&            // Rectangle's Right Vertical Edge passes through the hole AND
		rx2 < hx2 &&                //

		ry2 >= hy2                  // Rectangle's Bottom Horizontal Edge is below the hole
	) {
		newHoles.push_back(new SHAPE(nbHole++, rx2, hole->y, hx2-rx2, hy2-hole->y));
	}
	//

	// ⬜⬛⬛
	// ⬛⬛⬛
	// ⬛⬛⬛
	// [CASE 6]: Top Left Corner
	else if (
		rectangle->x <= hole->x &&  // Rectangle's Left Vertical Edge is left to the hole AND

		rectangle->y <= hole->y &&  // Rectangle's Top Horizontal Edge is above the hole AND

		rx2 > hole->x &&            // Rectangle's Right Vertical Edge passes through the hole AND
		rx2 < hx2 &&                // 

		ry2 > hole->y &&            // Rectangle's Bottom Horizontal Edge passes through the hole
		ry2 < hy2                   //
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, ry2, hx2-hole->x, hy2-ry2));
		newHoles.push_back(new SHAPE(nbHole++, rx2, hole->y, hx2-rx2, hy2-hole->y));
	}
	//

	// ⬛⬛⬜
	// ⬛⬛⬛
	// ⬛⬛⬛
	// [CASE 7]: Top Right Corner
	else if (
		rectangle->x > hole->x &&   // Rectangle's Left Vertical Edge Crosses in the hole AND
		rectangle->x < hx2 &&       // 

		rectangle->y <= hole->y &&  // Rectangle's Top Horizontal Edge is above the hole AND

		rx2 > hole->x &&            // Rectangle's Right Vertical Edge is right to the hole AND
		rx2 >= hx2 &&               // 

		ry2 > hole->y &&            // Rectangle's Bottom Horizontal Edge passes through the Hole
		ry2 < hy2                   //
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, rectangle->x-hole->x, hy2-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, hole->x, ry2, hx2-hole->x, hy2-ry2));
	}
	//

	// ⬛⬛⬛
	// ⬛⬛⬛
	// ⬛⬛⬜
	// [CASE 8]: Bottom Right Corner
	else if (
		rectangle->x < hx2 &&       // Rectangle's Left Vertical Edge passes through the hole AND
		rectangle->x > hole->x &&   //

		rectangle->y > hole->y &&   // Rectangle's Top Horizontal Edge passes through the hole AND
		rectangle->y < hy2 &&       //

		rx2 >= hx2 &&               // Rectangle's Right Vertical Edge is right to the hole AND

		ry2 >= hy2                  // Rectangle's Bottom Horizontal Edge is below the hole
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, hx2-hole->x, rectangle->y-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, rectangle->x-hole->x, hy2-hole->y));
	}

	// ⬛⬛⬛
	// ⬛⬛⬛
	// ⬜⬛⬛
	// [CASE 9]: Bottom Left Corner
	else if (
		rectangle->x <= hole->x &&  // Rectangle's Left Vertical Edge is left to the hole AND

		rectangle->y > hole->y &&   // Rectangle's Top Horizontal Edge passes through the hole AND
		rectangle->y < hy2 &&       //

		rx2 > hole->x &&            // Rectangle's Right Vertical Edge passes through the hole AND 
		rx2 < hx2 &&                // 

		ry2 >= hy2                  // Rectangle's Bottom Horizontal Edge passes through the hole
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, hx2-hole->x, rectangle->y-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, rx2, hole->y, hx2-rx2, hy2-hole->y));
	}

	// ⬛⬜⬛
	// ⬛⬜⬛
	// ⬛⬜⬛
	// [CASE 10]: Middle Vertical Bar
	else if (
		rectangle->x > hole->x &&   // Rectangle's Left Vertical Edge passes through the hole AND
		rectangle->x < hx2 &&       //

		rectangle->y <= hole->y &&  // Rectangle's Top Horizontal Edge is above the hole AND

		rx2 > hole->x &&            // Rectangle's Right Vertical Edge passes through the hole AND
		rx2 < hx2 &&                //

		ry2 >= hy2                  // Rectangle's Bottom Horizontal Edge is below the hole
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, rectangle->x-hole->x, hy2-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, rx2, hole->y, hx2-rx2, hy2-hole->y));
	}

	// ⬛⬛⬛
	// ⬜⬜⬜
	// ⬛⬛⬛
	// [CASE 11]: Middle Horizontal Bar
	else if (
		rectangle->x <= hole->x &&  // Rectangle's Left Vertical Edge is left to the hole AND

		rectangle->y > hole->y &&   // Rectangle's Top Horizontal Edge passes through the hole AND
		rectangle->y < hy2 &&       //

		rx2 >= hx2 &&               // Rectangle's Right Vertical Edge if right to the hole AND

		ry2 > hole->y &&            // Rectangle's Bottom Horizontal Edge is below the hole
		ry2 < hy2                   //
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, hx2-hole->x, rectangle->y-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, hole->x, ry2, hx2-hole->x, hy2-ry2));
	}

	// ⬛⬜⬛
	// ⬛⬛⬛
	// ⬛⬛⬛
	// [CASE 12]: Top Rectangle Pop-Out
	else if (
		rectangle->x > hole->x &&   // Rectangle's Left Vertical Edge passes through the hole AND
		rectangle->x < hx2 &&       //

		rectangle->y <= hole->y &&  // Rectangle's Top Horizontal Edge is above the hole AND

		rx2 > hole->x &&            // Rectangle's Right Vertical Edge pases through the hole AND
		rx2 < hx2 &&                //

		ry2 > hole->y && // Rectangle's Bottom Horizontal Edge passes through the hole
		ry2 < hy2    //
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, rectangle->x-hole->x, hy2-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, hole->x, ry2, hx2-hole->x, hy2-ry2));
		newHoles.push_back(new SHAPE(nbHole++, rx2, hole->y, hx2-rx2, hy2-hole->y));
	}

	// ⬛⬛⬛
	// ⬛⬛⬜
	// ⬛⬛⬛
	// [CASE 13]: Right Rectangle Pop-Out
	else if (
		rectangle->x > hole->x &&   // Rectangle's Left Vertical Edge passes through the hole AND
		rectangle->x < hx2 &&       //

		rectangle->y > hole->y &&   // Rectangle's Top Horizontal Edge passes through the hole AND
		rectangle->y < hy2 &&       //

		rx2 >= hx2 &&               // Rectangle's Right Vertical Edge is right to the hole AND

		ry2 > hole->y &&            // Rectangle's Bottom Horizontal Edge is below the hole
		ry2 < hy2                   //
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, hx2-hole->x, rectangle->y-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, rectangle->x-hole->x, hy2-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, hole->x, ry2, hx2-hole->x, hy2-ry2));
	}

	// ⬛⬛⬛
	// ⬛⬛⬛
	// ⬛⬜⬛
	// [CASE 14]: Bottom Rectangle Pop-Out
	else if (
		rectangle->x > hole->x &&   // Rectangle's Left Vertical Edge passes through the hole AND
		rectangle->x < hx2 &&       //

		rectangle->y > hole->y &&   // Rectangle's Top Horizontal Edge passes through the hole AND
		rectangle->y < hy2 &&       // 

		rx2 > hole->x &&            // Rectangle's Right Vertical Edge passes through the hole AND
		rx2 < hx2 &&                //

		ry2 >= hy2                  // Rectangle's Bottom Horizontal Edge is below the hole
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, rectangle->x-hole->x, hy2-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, hx2-hole->x, rectangle->y-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, rx2, hole->y, hx2-rx2, hy2-hole->y));
	}

	// ⬛⬛⬛
	// ⬜⬛⬛
	// ⬛⬛⬛
	// [CASE 15]: Left Rectangle Pop-Out
	else if (
		rectangle->x <= hole->x &&  // Rectangle's Left Vertical Edge is left to the hole AND

		rectangle->y > hole->y &&   // Rectangle's Top Horizontal Edge passes through the hole AND
		rectangle->y < hy2 &&       //

		rx2 > hole->x &&            // Rectangle's Bottom Horizontal Edge passes through the hole AND
		rx2 < hx2 &&                //

		ry2 > hole->y &&            // Rectangle's Right Vertical Edge passes through the hole
		ry2 < hy2                   //
	) {
		newHoles.push_back(new SHAPE(nbHole++, hole->x, hole->y, hx2-hole->x, rectangle->y-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, rx2, hole->y, hx2-rx2, hy2-hole->y));
		newHoles.push_back(new SHAPE(nbHole++, hole->x, ry2, hx2-hole->x, hy2-ry2));
	}

	// [SPECIAL CASE] Hole is covered by rectangle
	// do nothing

    if (!hole->isCovered(holes)) {
        for (auto newHole: newHoles) {
            if (!newHole->isCovered(holes)) {
                holes->insert(newHole);
            } else {
                delete newHole;
            }
        }
    } else {
        for (auto newHole: newHoles) {
            delete newHole;
        }
    }

}

void updateHoles(SHAPE*& rectangle, SHAPE_SET*& holes)
{
	// Cut holes
	SHAPE_SET* newHoles = new SHAPE_SET;

	for (SHAPE_SET::iterator it = holes->begin(); it != holes->end(); it++)
	{
		SHAPE* hole = *it;
		// If the Current Rectangle overlaps with a hole, we break the hole into new holes
		if (rectangle->intersects(hole))
		{
			cutHole(rectangle, hole, newHoles);
			delete hole;
		}
		else if (!(hole->isCovered(newHoles)))
		{
			newHoles->insert(hole);
		}
		else
		{
			delete hole;
		}
	}

	delete holes;
	holes = newHoles;

	// Merge adjacent holes
	// SHAPE_SET* newHoles2 = new SHAPE_SET;
	// for (SHAPE_SET::iterator it1 = holes->begin(); it1 != holes->end(); it1++)
	// {
	//     SHAPE* hole1 = *it1;
	//     for (SHAPE_SET::iterator it2 = std::next(it1); it2 != holes->end(); it2++)
	//     {
	//         SHAPE* hole2 = *it2;

	//         // ⬜ = hole1
	//         // ⬛ = hole2
	//         if (hole1->y == hole2->y && hole1->h == hole2->h)
	//         {
	//             // ⬜⬛
	//             if (hole1->getx2() == hole2->x)
	//             { newHoles2->insert(new SHAPE(nbHole++, hole1->x, hole1->y, hole1->w+hole2->w, hole1->h)); break; }
	//             // ⬛⬜
	//             else if (hole2->getx2() == hole1->x)
	//             { newHoles2->insert(new SHAPE(nbHole++, hole2->x, hole2->y, hole1->w+hole2->w, hole2->h)); break; }
	//         }
	//         else if (hole1->x == hole2->x && hole1->w == hole2->w)
	//         {
	//             // ⬜
	//             // ⬛
	//             if (hole1->gety2() == hole2->y)
	//             { newHoles2->insert(new SHAPE(nbHole++, hole1->x, hole1->y, hole1->w, hole1->h+hole2->h)); break; }
	//             // ⬛
	//             // ⬜
	//             else if (hole2->gety2() == hole1->y)
	//             { newHoles2->insert(new SHAPE(nbHole++, hole2->x, hole2->y, hole2->w, hole1->h+hole2->h)); break; }
	//         }

	//         newHoles2->insert(hole1);
	//         newHoles2->insert(hole2);
	//     }
	// }
	// holes = newHoles2;
}

// getNewHeight : gets y2 of rectangle if we were to place it in hole
int getNewHeight(SHAPE*& hole, SHAPE*& rectangle)
{
	return hole->y + rectangle->h;
}

// Differentiation function for getBestHole, it say if this new hole is better than the current best hole
bool isBetterHole(SHAPE*& rectangle, SHAPE*& hole, SHAPE*& bestHole, int bestHeight)
{
	if (rectangle->fitsIn(hole)) // Rectangle fits in the hole
	{
		if (!bestHole)
			return true;

		int height = getNewHeight(hole, rectangle);

		bool holePerfect        = (rectangle->w == hole->w && rectangle->h == hole->h);
		bool bestHolePerfect    = (rectangle->w == bestHole->w && rectangle->h == bestHole->h);

		// Best hole is a perfect fit ...
		if (bestHolePerfect && !holePerfect)
			return false;

		// Hole is a perfect fit !
		if (holePerfect && !bestHolePerfect)
			return true;

		// Differentiate holes with equal properties to get a deterministic result
		if (height < bestHeight) return true;
		if (height != bestHeight) return false;

		// if ((hole->h-rectangle->h) > (besthole->h-rectangle->h)) return true;
		// if ((hole->h-rectangle->h) != (besthole->h-rectangle->h)) return false;

		if (hole->y < bestHole->y) return true;
		if (hole->y != bestHole->y) return false;

		if (hole->x < bestHole->x) return true;
		if (hole->x != bestHole->x) return false;
		if (hole->h < bestHole->h) return true;
		if (hole->h == bestHole->h) return false;
		if (hole->w < bestHole->w) return true;
		if (hole->w != bestHole->w) return false;
		if (hole->id < bestHole->id) return true;
	}
	return false;
}

// Find the best hole to place our rectangle in
SHAPE* getBestHole(SHAPE*& rectangle, SHAPE_SET*& holes, bool rotations)
{
	SHAPE* bestHole = nullptr;
	int bestHeight = INT_INFINITY;
	bool doRotation = false;
	
	for (SHAPE_SET::iterator it = holes->begin(); it != holes->end(); it++)
	{
		SHAPE* hole = *it;

		// Normal Rotation
		if (isBetterHole(rectangle, hole, bestHole, bestHeight))
		{
			bestHeight = getNewHeight(hole, rectangle);
			bestHole = hole;
			doRotation = false;
		}

		// Rotated Solution
		if (!rotations) // Don't explore the rotated solution if rotations are set to false
			continue;

		rectangle->rotate(); // Rotate
		if (isBetterHole(rectangle, hole, bestHole, bestHeight))
		{
			bestHeight = getNewHeight(hole, rectangle);
			bestHole = hole;
			doRotation = true;
		}
		rectangle->rotate(); // Rotate back to original (we saved result before)
	}

	if (doRotation) // Remember our saved variable indicating if the best solution is a rotated solution
		rectangle->rotate();
	
	return bestHole;
}

bool fewNeighborsOnLeft(SHAPE*& rectangle, SHAPE_VEC*& rectangles)
{
	if (rectangle->x == 0)
		return true;

	int length = 0;

	for (SHAPE_VEC::iterator it = rectangles->begin(); it != rectangles->end(); it++)
	{
		SHAPE* rectangle2 = *it;
		if (rectangle == rectangle2) 
			continue;
		if (rectangle2->getx2() != rectangle->x)
			continue;
		if (rectangle2->gety2() <= rectangle->y)
			continue;
		if (rectangle2->y >= rectangle->gety2())
			continue;

		if (
			(rectangle2->y <= rectangle->y) &&
			(rectangle2->gety2() >= rectangle->gety2())
		) return true;

		if (
			(rectangle2->y <= rectangle->y) &&
			(rectangle2->gety2() <= rectangle->gety2())
		) length += rectangle2->gety2() - rectangle->y;

		if (
			(rectangle2->y >= rectangle->y) &&
			(rectangle2->gety2() >= rectangle->gety2())
		) length += rectangle->gety2() - rectangle2->y;
	}

	if (length > rectangle->h/2)
		return true;

	return false;
}

void printPerc(float n, float o) // print n out of o
{
	std::cout << "\r" << "Progress: " << n << "/" << o;
}

// PACKER::solve
RESULT* solve(int W, SHAPE_VEC*& rectangles, bool rotations, HEURISTIC strategy, bool showProgress)
{
	// Initializations
	RESULT* result = new RESULT;
		result->w = W;
		result->rotations = rotations;
		result->sortStrategy = strategy;

	SHAPE_SET* holes = new SHAPE_SET;

	// Start Hole is the width of the entire canvas + an irrelevant height
	holes->insert(new SHAPE(1, 0, 0, W, INT_INFINITY));

	// Time
	auto start = std::chrono::high_resolution_clock::now();

	// Sort based on heuristic
	switch (strategy)
	{
		case HEURISTIC::descArea:
			std::sort(rectangles->begin(), rectangles->end(), descendingArea); break;
		case HEURISTIC::descArea2:
			std::sort(rectangles->begin(), rectangles->end(), descendingArea2); break;
		case HEURISTIC::descWidth:
			std::sort(rectangles->begin(), rectangles->end(), descendingWidth); break;
		case HEURISTIC::descHeight:
			std::sort(rectangles->begin(), rectangles->end(), descendingHeight); break;
		case HEURISTIC::ascArea:
			std::sort(rectangles->begin(), rectangles->end(), ascendingArea); break;
		case HEURISTIC::ascWidth:
			std::sort(rectangles->begin(), rectangles->end(), ascendingWidth); break;
		case HEURISTIC::ascHeight:
			std::sort(rectangles->begin(), rectangles->end(), ascendingHeight); break;
	}

	int A = 0; // Total rectangle area
	int maxH = 0; // Maximum height of all rectangle height's
	int H = 0;

	int n = 0,                  // Current rectangle
		N = rectangles->size(); // Nb of rectangles
	
	// Verbose
	if (showProgress)
	{
		std::cout << '\n'; 
		printPerc(n, N);
	}

	// Main iteration to place 'rectangles'
	for (SHAPE_VEC::iterator it = rectangles->begin(); it != rectangles->end(); it++)
	{
		SHAPE* rectangle = *it;
		SHAPE* hole = getBestHole(rectangle, holes, rotations);

		A += rectangle->getArea();
		maxH = std::max(maxH, rotations ? std::min(rectangle->w, rectangle->h) : rectangle->h);

		if (hole)
		{
			// Place rectangle in best hole to top-left
			rectangle->x = hole->x;
			rectangle->y = hole->y;

			// If no rectangle on its left then move it to the right -> bigger hole on its left
			if (!fewNeighborsOnLeft(rectangle, rectangles))
				rectangle->x = hole->getx2() - rectangle->w;

			// Update new height
			H = std::max(H, getNewHeight(hole, rectangle));

			// Update the holes
			updateHoles(rectangle, holes);
		}
		else
		{
			std::cerr << '\n' << "Error: No Hole for rectangle " << rectangle->id << '\n';
			return nullptr;
		}

		n++;
		if (showProgress)
			printPerc(n, N);
	}
	if (showProgress) std::cout << '\n';

	auto end = std::chrono::high_resolution_clock::now();

	// Save result
	result->h = H;
	result->opt_h = std::max(round(float(A) / float(W)), float(maxH));
	float canvasArea = result->w*result->h;
	result->loss = (canvasArea-float(A))/canvasArea * 100.f;
	result->time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	result->rectangles = rectangles;

	for (auto hole : *holes) {
        delete hole;
    }
    delete holes;

	#if CHECK_VALID
		bool passedCheck = true;
		for (SHAPE_VEC::iterator it1 = rectangles->begin(); it1 != rectangles->end(); it1++)
		{
			SHAPE* rectangle1 = *it1;
			for (SHAPE_VEC::iterator it2 = rectangles->begin(); it != rectangles->end(); it2++)
			{
				SHAPE* rectangle2 = *it2;
				if (!(rectangle1 == rectangle2) && rectangle1->intersects(rectangle2))
					passedCheck = false;

				if (!passedCheck) break;
			}
			if (!passedCheck) break;
		}
		std::cout << "Solution is " << (passedCheck ? "valid" : "not valid") << '\n';
	#endif

	return result;
}