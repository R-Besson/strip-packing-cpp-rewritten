#ifndef INSTANCE_GEN_H
#define INSTANCE_GEN_H

#include "../types.h"

std::vector<Shape> gen_instance(uint32_t W, uint32_t N, float ratio, std::mt19937 &engine);

#endif