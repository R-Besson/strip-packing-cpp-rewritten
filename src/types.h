/**==============================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Type/Class definitions for strip packing
 *=============================================**/

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_set>
#include <unordered_map>

/**============================================
 *         Sorting Strategy Heuristic
 *=============================================**/
enum HEURISTIC
{
	descArea,   // Prefers Bigger Area
	descArea2,   // Prefers Bigger Area
	descWidth,  // Prefers Bigger Width
	descHeight, // Prefers Bigger Height
	ascArea,    // Prefers Smaller Area
	ascWidth,   // Prefers Smaller Area
	ascHeight   // Prefers Smaller Height
};

/**============================================
 *          SHAPE CLASS (x,y,w,h,...)
 *=============================================**/
class SHAPE
{
private:
	typedef std::vector<SHAPE*> SHAPE_VEC; // Vector of SHAPE pointers
	typedef std::unordered_set<SHAPE*> SHAPE_SET; // Set of SHAPE pointers
	typedef std::unordered_map<int, SHAPE_SET*> SHAPE_MAP; // Map of <int, SHAPE pointers>
public:
	// Properties
	int id;

	int x, y; // Top Left Corner (Position)
	int w, h; // Width, Height (Dimensions)
	int a;

	bool isRotated = false;

	// Functions
	int getx2();
	int gety2();
	int getArea();

	void rotate();
	bool fitsIn(SHAPE*&);
	bool isIn(SHAPE*&);
	bool intersects(SHAPE*&);
	bool isCovered(SHAPE_SET*&);

	bool operator==(SHAPE&);

	// Constructors
	explicit SHAPE(int id_, int w_, int h_)
	: id(id_), x(0), y(0), w(w_), h(h_), a(0) {}

	explicit SHAPE(int x_, int y_, int w_, int h_)
	: id(0), x(x_), y(y_), w(w_), h(h_), a(0) {} 

	explicit SHAPE(int id_, int x_, int y_, int w_, int h_)
	: id(id_), x(x_), y(y_), w(w_), h(h_), a(0) {}
};

typedef std::vector<SHAPE*> SHAPE_VEC; // Vector of SHAPE pointers
typedef std::unordered_set<SHAPE*> SHAPE_SET; // Set of SHAPE pointers
typedef std::unordered_map<int, SHAPE_SET*> SHAPE_MAP; // Map of <int, SHAPE pointers>

/**============================================
 *               RESULT CLASS
 * contains info on the result of the packing
 *=============================================**/
class RESULT
{   public:

	int w, h;               // Width & Height of container/canvas
	int opt_h;              // Theoretical Optimal Height -> opt_h = totalRectArea / w
	HEURISTIC sortStrategy; // Initial Sort Method used to sort rectangles
	float loss;             // Canvas loss as percentage -> loss = (containerArea - totalRectArea) / (containerArea);
	bool rotations;         // Were rotations allowed?

	long long time;         // in ms

	SHAPE_VEC* rectangles;       // vector with packed rects (x/y's changed) and sorted by ascending id
};

#endif