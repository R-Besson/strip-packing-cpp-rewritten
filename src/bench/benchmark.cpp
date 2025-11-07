/**====================================================
 * @author      : Romain BESSON
 * @description : Random Instance Benchmarking
 *=====================================================**/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <cmath>
#include <random>
#include <map>

#include "../cxxopts.hpp"     // CXXOpts for argument parsing
#include "instance_gen.h"	  // 2D SPP Instance Generator
#include "../packer/packer.h" // 2D Packing Library

void print_args(uint32_t iterations, uint32_t N, float ratio, const std::string &output_file, bool verbose, uint32_t width, bool rotations, Heuristic strategy)
{
	std::cout << "\nBenching with:\n";
	std::cout << "> Iteration Count:    " << iterations << "\n";
	std::cout << "> Rectangle Count:    " << N << "\n";
	std::cout << "> Ratio Height/Width: " << ratio << "\n";
	std::cout << "> Strip Width:        " << width << "\n";
	std::cout << "> Rotations Allowed:  " << (rotations ? "Yes" : "No") << "\n";
	std::cout << "> Solve Strategy:     " << HeuristicStrings.at(strategy) << "\n";
	std::cout << "> Output File:        " << (output_file.empty() ? "None" : output_file) << "\n";
	std::cout << "> Verbose:            " << (verbose ? "Yes" : "No") << "\n\n";
}

// Round to specified number of decimal digits
double keep_digits(double value, uint32_t digits)
{
	double precision = std::pow(10.0, digits);
	return std::round(value * precision) / precision;
}

int main(int argc, char *argv[])
{
	cxxopts::Options options("bench", "A 2D SPP benchmark tool for randomly generated instances.");

	options.add_options()
		("h,help", "Print usage information")
		("o,output", "Optional CSV file to save results", cxxopts::value<std::string>())
		("v,verbose", "Show progress for each iteration", cxxopts::value<bool>()->default_value("false"))
		("w,width", "Width of the strip for packing", cxxopts::value<uint32_t>()->default_value("10000"))
		("r,rotate", "Allow rectangles to be rotated during packing", cxxopts::value<bool>()->default_value("false"))
		("s,strategy", "Heuristic for sorting rectangles (0-3) (Desc. Area | Desc. Area 2 | Desc Width | Desc Height)", cxxopts::value<int>()->default_value("3"))
		("iterations", "Number of benchmark iterations to run", cxxopts::value<uint32_t>())
		("rects", "Number of rectangles per instance", cxxopts::value<uint32_t>())
		("ratio", "Height/width ratio for the initial area", cxxopts::value<float>());
	options.positional_help("<iterations> <rects> <ratio>");
	options.parse_positional({"iterations", "rects", "ratio"});
	
	cxxopts::ParseResult result;
	try {
		result = options.parse(argc, argv);
	} catch (const cxxopts::exceptions::exception& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << options.help() << std::endl;
		return EXIT_FAILURE;
	}
	if (result.count("help")) {
		std::cout << options.help() << std::endl;
		return EXIT_SUCCESS;
	}
	if (result.count("iterations") == 0 || result.count("rects") == 0 || result.count("ratio") == 0) {
		std::cerr << "Error: Missing required arguments <iterations>, <rects>, and <ratio>.\n";
		std::cout << options.help() << std::endl;
		return EXIT_FAILURE;
	}

	// Get arguments from parsed results
	uint32_t iterations = result["iterations"].as<uint32_t>();
	uint32_t N = result["rects"].as<uint32_t>();
	float ratio = result["ratio"].as<float>();
	bool verbose = result["verbose"].as<bool>();
	uint32_t width = result["width"].as<uint32_t>();
	bool rotations = result["rotate"].as<bool>();
	auto strategy = static_cast<Heuristic>(std::clamp(result["strategy"].as<int>(), 0, static_cast<int>(Heuristic::Count)-1));
	std::string output_file = result.count("output") ? result["output"].as<std::string>() : "";

	// Post-parsing validation
	if (iterations == 0) {
		std::cerr << "Error: Iteration count must be greater than 0.\n";
		return EXIT_FAILURE;
	}
	if (ratio <= 0) {
		std::cerr << "Error: Ratio must be a positive number.\n";
		return EXIT_FAILURE;
	}

	print_args(iterations, N, ratio, output_file, verbose, width, rotations, strategy);

	double best = std::numeric_limits<double>::infinity();
	double worst = -std::numeric_limits<double>::infinity();
	double sum = 0.0;

	if (verbose)
		std::cout << "Starting benchmark...\n";

	std::random_device seeder;
	std::mt19937 engine(seeder());

	std::ofstream ofs;
	if (!output_file.empty()) {
		ofs.open(output_file);
		if (!ofs.is_open()) {
			std::cerr << "Error: Cannot open file '" << output_file << "' for writing.\n";
			return EXIT_FAILURE;
		}
		ofs << "#IT,H,OPT_H,H_div_OPT_H\n"; // CSV header
	}

	for (uint32_t i = 1; i <= iterations; ++i) {
		std::vector<Shape> rectangles = gen_instance(width, N, ratio, engine);
		Result pack_result = solve(width, rectangles, rotations, strategy, false);

		const double expected_h = static_cast<double>(width) * ratio;
		const double alpha = keep_digits(static_cast<double>(pack_result.h) / expected_h, 4);

		best = std::min(best, alpha);
		worst = std::max(worst, alpha);
		sum += alpha;

		if (verbose)
			std::cout << "IT " << std::setw(4) << i << "/" << iterations
					  << " -> H=" << std::setw(6) << pack_result.h
					  << ", Ratio=" << std::fixed << std::setprecision(4) << alpha << "\n";

		if (ofs.is_open())
			ofs << i << ',' << pack_result.h << ',' << static_cast<uint32_t>(expected_h) << ',' << alpha << '\n';
	}

	const double average = sum / iterations;

	std::cout << "\nDone!\n";
	std::cout << "Worst Ratio: " << worst << "\n";
	std::cout << "Best Ratio:  " << best << "\n";
	std::cout << "Avg Ratio:   " << average << "\n";

	if (ofs.is_open()) {
		ofs << "Summary: worst=" << worst << ",best=" << best << ",avg=" << average << '\n';
	}

	return EXIT_SUCCESS;
}