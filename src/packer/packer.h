#ifndef PACKER_H
#define PACKER_H

#include "../types.h"

Result solve(uint32_t W, std::vector<Shape> rectangles, bool rotations, Heuristic strategy, bool show_progress);

#endif