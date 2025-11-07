/**==============================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Type/Class definitions for strip packing
 *=============================================**/

#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <vector>
#include <string>
#include <chrono>
#include <optional>
#include <functional>

/**============================================
 *          Shape class (x,y,w,h,...)
 *=============================================**/
class Shape {
private:
	uint32_t id_ = 0;
	uint32_t x_ = 0;
	uint32_t y_ = 0;
	uint32_t w_ = 0;
	uint32_t h_ = 0;
	mutable std::optional<uint32_t> area_;
	bool is_rotated_ = false;

public:
	Shape(uint32_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
		: id_(id), x_(x), y_(y), w_(w), h_(h)
	{}

	uint32_t id() const { return id_; }
	uint32_t x() const { return x_; }
	uint32_t y() const { return y_; }
	uint32_t w() const { return w_; }
	uint32_t h() const { return h_; }
	uint32_t x2() const { return x_ + w_; }
	uint32_t y2() const { return y_ + h_; }
	bool is_rotated() const { return is_rotated_; }
	
	// Mutators
	uint32_t area() const; // stores calculated area in area_
	void set_position(uint32_t new_x, uint32_t new_y) { x_ = new_x; y_ = new_y; }
	void rotate();

	// Geometric queries
	bool fits_in(const Shape &container) const;
	bool is_in(const Shape &container) const;
	bool intersects(const Shape &other) const;
	bool is_covered(const std::vector<Shape> &others) const;

	bool operator==(const Shape &other) const;
};

/**============================================
 *         Sorting Strategy Heuristic
 *=============================================**/
enum class Heuristic
{
	DescendingArea,
	DescendingArea2,
	DescendingWidth,
	DescendingHeight,
	
	Count
};

const std::map<Heuristic, std::string> HeuristicStrings = {
	{Heuristic::DescendingArea, "Descending Area"},
	{Heuristic::DescendingArea2, "Descending Area 2"},
	{Heuristic::DescendingWidth, "Descending Width"},
	{Heuristic::DescendingHeight, "Descending Height"},
};

/**============================================
 *               Result structure
 * contains info on the result of the packing
 *=============================================**/
struct Result
{
	uint32_t w = 0, h = 0;                               // Width & Height of container/canvas
	uint32_t opt_h = 0;                                  // Theoretical Optimal Height -> opt_h = totalRectArea / w
	Heuristic sort_strategy = Heuristic::DescendingArea; // Initial Sort Method used to sort rectangles
	float loss = 0.0;                                    // Canvas loss as percentage -> loss = (containerArea - totalRectArea) / (containerArea);
	bool rotations = false;                              // Were rotations allowed
	std::vector<Shape> rectangles{};                     // vector with packed rects (x/y's changed) and sorted by ascending id
	long long elapsed_ms{};
};

#endif
