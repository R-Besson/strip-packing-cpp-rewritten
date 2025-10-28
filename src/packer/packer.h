#include "../types.h"

#ifndef PACKER_H
#define PACKER_H

// <width> <unordered_rectangles> <rotations> <sort_strategy> <show_progress/verbose>
RESULT* solve(int, SHAPE_VEC*&, bool, HEURISTIC, bool);

#endif