/**==============================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Main exe that solves SPP
 *                from list of rectangles
 *                in format <W> <H> (new line)
 *=============================================**/

#include <iostream>
#include <fstream>

#include "packer/packer.h"		   // 2D Packing Library
#include "visualizer/visualizer.h" // 2D Visualizing Library
#include "cxxopts.hpp"			   // CXXOpts for argument parsing

void print_args(const std::string &input_file, uint32_t W, bool rotations, Heuristic strategy, bool show_progress, const std::string &output_file)
{
	std::cout << '\n'
			  << "Solving with:" << '\n';
	std::cout << "> Input File:      " << input_file << '\n';
	std::cout << "> Width:           " << W << '\n';
	std::cout << "> Rotations:       " << (rotations ? "Yes" : "No") << '\n';
	std::cout << "> Sort Strategy:   " << HeuristicStrings.at(strategy) << '\n';
	std::cout << "> Show Progress:   " << (show_progress ? "Yes" : "No") << '\n';
	std::cout << "> Output File:     " << (output_file.empty() ? "None" : output_file) << '\n';
}

void print_result(const Result &result)
{
	std::cout << '\n'
			  << "Result:" << '\n';
	std::cout << "> Time Taken:                   " << result.elapsed_ms << "ms" << '\n';
	std::cout << "> Solution Height:              " << result.h << '\n';
	std::cout << "> Theoretical Optimal Height:   " << result.opt_h << '\n';
	std::cout << "> Ratio SOLUTION/OPTIMAL:       " << static_cast<float>(result.h) / result.opt_h << '\n';
	std::cout << "> Loss:                         " << result.loss << '%' << '\n';
}

std::string get_font_path(const std::string &exe_path_str)
{
	std::filesystem::path exe_path(exe_path_str);
	return (exe_path.parent_path() / "anon.ttf").string();
}

int main(int argc, char *argv[])
{
	cxxopts::Options options("packer", "A 2D strip packing problem (SPP) solver.");

	options.add_options()
		("h,help", "Print usage information")
		("r,rotate", "Allow rectangles to be rotated", cxxopts::value<bool>()->default_value("false"))
		("v,verbose", "Show packing progress", cxxopts::value<bool>()->default_value("false"))
		("s,strategy", "Heuristic for sorting rectangles (0-3) (Desc. Area | Desc. Area 2 | Desc Width | Desc Height)", cxxopts::value<int>()->default_value("3"))
		("a,all", "Solve using all heuristics and output the best result.", cxxopts::value<bool>()->default_value("false"))
		("o,output", "Output CSV file name", cxxopts::value<std::string>())
		("input-file", "Input rectangles file (format: <w> <h> per line)", cxxopts::value<std::string>())
		("width", "The width of the strip for packing", cxxopts::value<uint32_t>());
	options.positional_help("<input-file> <width>");
	options.parse_positional({"input-file", "width"});

	cxxopts::ParseResult result;
	try
	{
		result = options.parse(argc, argv);
	}
	catch (const cxxopts::exceptions::exception &e)
	{
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << options.help() << std::endl;
		return EXIT_FAILURE;
	}
	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		return EXIT_SUCCESS;
	}
	if (result.count("input-file") == 0 || result.count("width") == 0)
	{
		std::cerr << "Error: Missing required arguments <rectangles_file> and <width>.\n";
		std::cout << options.help() << std::endl;
		return EXIT_FAILURE;
	}

	// Get Args from parsed results
	std::string exe_path = argv[0];
	std::string input_file = result["input-file"].as<std::string>();
	uint32_t W = result["width"].as<uint32_t>();
	bool rotations = result["rotate"].as<bool>();
	bool verbose = result["verbose"].as<bool>();
	auto strategy = static_cast<Heuristic>(std::clamp(result["strategy"].as<int>(), 0, static_cast<int>(Heuristic::Count)-1));
	bool all_heuristics = result["all"].as<bool>();

	std::string output_file;
	if (result.count("output"))
	{
		output_file = result["output"].as<std::string>();
	}

	// Reading input file
	std::vector<Shape> rectangles{};
	std::ifstream ifs(input_file);
	if (!ifs.is_open())
	{
		std::cerr << '\n'
				  << "Error: Couldn't open file " << input_file << ": " << std::strerror(errno) << '\n';
		return EXIT_FAILURE;
	}
	uint32_t id = 0;
	uint32_t w, h;
	while (ifs >> w >> h)
	{
		rectangles.push_back(Shape(++id, 0, 0, w, h));
	}

	// Solve
	Result pack_result;
	if (all_heuristics)
	{
		std::cout << '\n'
		<< "Solving with all heuristics to find the best result..." << '\n';
		std::cout << "> Input File:      " << input_file << '\n';
		std::cout << "> Width:           " << W << '\n';
		std::cout << "> Rotations:       " << (rotations ? "Yes" : "No") << '\n';
		std::cout << "> Show Progress:   " << (verbose ? "Yes" : "No") << '\n';
		std::cout << "> Output File:     " << (output_file.empty() ? "None" : output_file) << "\n\n";
		
		Result best_result;
		best_result.h = UINT32_MAX;
		
		for (int i = 0; i < static_cast<int>(Heuristic::Count); ++i)
		{
			Heuristic current_strategy = static_cast<Heuristic>(i);
			std::cout << "Testing Strategy: " << HeuristicStrings.at(current_strategy) << " ...\n";
			
			std::vector<Shape> rectangles_copy = rectangles;
			
			Result current_result = solve(W, rectangles_copy, rotations, current_strategy, verbose);
			
			std::cout << "  > Result Height: " << current_result.h << " (Time: " << current_result.elapsed_ms << "ms)\n";
			
			if (current_result.h < best_result.h)
			{
				best_result = current_result;
				std::cout << "  > New best result found!\n";
			}
		}
		pack_result = best_result;
		std::cout << "\nBest result found using strategy: \"" << HeuristicStrings.at(pack_result.sort_strategy) << "\"\n";
	}
	else
	{
		print_args(input_file, W, rotations, strategy, verbose, output_file);
		pack_result = solve(W, rectangles, rotations, strategy, verbose);
	}

	print_result(pack_result);

	// Write to output file
	if (!output_file.empty())
	{
		std::ofstream ofs(output_file);
		if (!ofs.is_open())
		{
			std::cerr << "Error: Cannot open file '" << output_file << "' for writing.\n";
		}
		else
		{
			ofs << "W=" << pack_result.w << ",H=" << pack_result.h << ",OPT(I)=" << pack_result.opt_h << '\n';
			ofs << "SORT=" << HeuristicStrings.at(pack_result.sort_strategy) << ",LOSS=" << pack_result.loss << '%' << ",rotations=" << pack_result.rotations << '\n';
			ofs << "id,x,y,w,h" << '\n';

			for (const Shape &rectangle : pack_result.rectangles)
			{
				ofs << rectangle.id() << "," << rectangle.x() << "," << rectangle.y() << "," << rectangle.w() << "," << rectangle.h() << "\n";
			}
		}
	}

	// Visualize result
	visualize(pack_result, 1280, 720, get_font_path(exe_path));

	return EXIT_SUCCESS;
}