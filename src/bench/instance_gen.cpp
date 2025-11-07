/**===============================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Guillotine-cuttable instance generator for SPP.
 *                Generates N rectangles by starting with one and
 *                recursively splitting existing ones.
 *================================================**/

#include <iostream>
#include <random>

#include "instance_gen.h"

uint32_t rand_range(uint32_t min, uint32_t max, std::mt19937 &engine)
{
	if (min > max)
	{
		return min;
	}
	return std::uniform_int_distribution<uint32_t>(min, max)(engine);
}

// Generate Random SPP Instance
std::vector<Shape> gen_instance(uint32_t W, uint32_t N, float ratio, std::mt19937 &engine)
{
	if (N == 0) return {};
	if (W == 0) return {};

	std::vector<Shape> rectangles;
	rectangles.reserve(N);
	rectangles.emplace_back(1, 0, 0, W, static_cast<uint32_t>(static_cast<float>(W) * ratio));

	bool horizontal_split = true;

	// need to create N-1 new rectangles
	for (uint32_t n = 1; n < N; ++n)
	{
		std::vector<size_t> splittable_indices;
		for (size_t i = 0; i < rectangles.size(); ++i)
		{
			if ((horizontal_split && rectangles[i].w() > 1) || (!horizontal_split && rectangles[i].h() > 1))
			{
				splittable_indices.push_back(i);
			}
		}

		if (splittable_indices.empty())
		{
			horizontal_split = !horizontal_split;
			for (size_t i = 0; i < rectangles.size(); ++i)
			{
				if ((horizontal_split && rectangles[i].w() > 1) || (!horizontal_split && rectangles[i].h() > 1))
				{
					splittable_indices.push_back(i);
				}
			}
		}

		if (splittable_indices.empty())
		{
			std::cerr << "Warning: Could not generate " << N << " rectangles. Stuck at "
					  << rectangles.size() << ". All remaining rectangles are too small to split.\n";
			break;
		}

		std::uniform_int_distribution<size_t> dist(0, splittable_indices.size() - 1);
		size_t index_to_split = splittable_indices[dist(engine)];

		Shape to_split = rectangles[index_to_split];

		if (horizontal_split)
		{
			uint32_t cut = rand_range(1, to_split.w() - 1, engine);
			rectangles[index_to_split] = Shape(to_split.id(), to_split.x(), to_split.y(), cut, to_split.h());
			rectangles.emplace_back(n + 1, to_split.x() + cut, to_split.y(), to_split.w() - cut, to_split.h());
		}
		else // Vertical split
		{
			uint32_t cut = rand_range(1, to_split.h() - 1, engine);
			rectangles[index_to_split] = Shape(to_split.id(), to_split.x(), to_split.y(), to_split.w(), cut);
			rectangles.emplace_back(n + 1, to_split.x(), to_split.y() + cut, to_split.w(), to_split.h() - cut);
		}

		horizontal_split = !horizontal_split;
	}

	return rectangles;
}