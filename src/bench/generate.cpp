/**====================================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Guillotine-cuttable instance generator interface
 *=====================================================**/

#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include "../cxxopts.hpp"
#include "instance_gen.h"

void print_args(uint32_t W, uint32_t N, float height_width_ratio, const std::string& output_file)
{
	std::cout << "\nGenerating with:\n";
	std::cout << "> Width:              " << W << '\n';
	std::cout << "> Rectangle Count:    " << N << '\n';
	std::cout << "> Ratio Height/Width: " << height_width_ratio << '\n';
	std::cout << "> Output File:        " << output_file << '\n';
}

int main(int argc, char *argv[])
{
	cxxopts::Options options("generate", "A generator for 2D strip packing problem (SPP) instances.");

	options.add_options()
		("h,help", "Print usage information")
		("width", "The width of the main container", cxxopts::value<uint32_t>())
		("rects", "The number of rectangles to generate", cxxopts::value<uint32_t>())
		("ratio", "The height/width ratio for the initial rectangle area", cxxopts::value<float>())
		("output", "The path to the output file", cxxopts::value<std::string>());
	options.positional_help("<width> <rects> <ratio> <output>");
	options.parse_positional({"width", "rects", "ratio", "output"});

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
	if (result.count("width") == 0 || result.count("rects") == 0 || result.count("ratio") == 0 || result.count("output") == 0) {
		std::cerr << "Error: Missing one or more required arguments.\n";
		std::cout << options.help() << std::endl;
		return EXIT_FAILURE;
	}

	// Get args from the parsed result
	uint32_t W = result["width"].as<uint32_t>();
	uint32_t N = result["rects"].as<uint32_t>();
	float height_width_ratio = result["ratio"].as<float>();
	std::string output_file = result["output"].as<std::string>();

	// Post-parsing validation
	if (W == 0) {
		std::cerr << "Error: Width cannot be zero.\n";
		return EXIT_FAILURE;
	}
	if (N == 0) {
		std::cerr << "Error: Rectangle count cannot be zero.\n";
		return EXIT_FAILURE;
	}
	if (height_width_ratio <= 0) {
		std::cerr << "Error: Ratio must be a positive number.\n";
		return EXIT_FAILURE;
	}

	print_args(W, N, height_width_ratio, output_file);

	// Gen instance
	std::random_device seeder;
	std::mt19937 engine(seeder());
	std::vector<Shape> rectangles = gen_instance(W, N, height_width_ratio, engine);

	std::ofstream ofs(output_file);
	if (!ofs.is_open())
	{
		std::cerr << "Error: Cannot open file '" << output_file << "' for writing.\n";
		return EXIT_FAILURE;
	}
	
	for (const Shape &rectangle : rectangles) {
		ofs << rectangle.w() << ' ' << rectangle.h() << '\n';
	}
	
	std::cout << "\nSuccessfully generated " << rectangles.size() << " rectangles to '" << output_file << "'\n";

	return EXIT_SUCCESS;
}