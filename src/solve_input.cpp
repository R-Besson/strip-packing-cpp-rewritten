/**==============================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Main exe that solves SPP
 *                from list of rectangles
 *                in format <W> <H> (new line)
 *=============================================**/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>

#include "packer/packer.h"          // 2D Packing Library
#include "visualizer/visualizer.h"  // 2D Visualizing Library

// Denominate enums as comprehensible strings
std::map<HEURISTIC, std::string> strategies = {
	{HEURISTIC::descArea, "Descending Area (0)"},
	{HEURISTIC::descArea2, "Descending Area (1)"},
	{HEURISTIC::descWidth, "Descending Width (2)"},
	{HEURISTIC::descHeight, "Descending Height (3)"},
	{HEURISTIC::ascArea, "Ascending Area (4)"},
	{HEURISTIC::ascWidth, "Ascending Width (5)"},
	{HEURISTIC::ascHeight, "Ascending Height (6)"}
};

void printUsage()
{
	std::cout << '\n' << "Usage (* = optional, use a '.' for default value):" << '\n';
	std::cout << "   <exe> <rectangles_file> <width> <can_rotate*> <sort_strategy*> <show_progress*> <output_file*>" << '\n' << '\n';
	std::cout << "Examples:" << '\n';
	std::cout << "   pack list.txt                      --> Imports data (format : <width> <height>)" << '\n';
	std::cout << "   pack ???????? 100                  --> Width = 100" << '\n';
	std::cout << "   pack ???????? ??? 0                --> non-rotatable solution (default)" << '\n';
	std::cout << "   pack ???????? ??? 1                --> rotatable solution" << '\n';
	std::cout << "   pack ???????? ??? ? 0              --> Sort Strategy = Descending Area" << '\n';
	std::cout << "   pack ???????? ??? ? 1              --> Sort Strategy = Descending Area 2" << '\n';
	std::cout << "   pack ???????? ??? ? 2              --> Sort Strategy = Descending Width" << '\n';
	std::cout << "   pack ???????? ??? ? 3              --> Sort Strategy = Descending Height (default)" << '\n';
	std::cout << "   pack ???????? ??? ? 4              --> Sort Strategy = Ascending Area" << '\n';
	std::cout << "   pack ???????? ??? ? 5              --> Sort Strategy = Ascending Width" << '\n';
	std::cout << "   pack ???????? ??? ? 6              --> Sort Strategy = Ascending Height" << '\n';
	std::cout << "   pack ???????? ??? ? 6 1            --> Verbose (default : true)" << '\n';
	std::cout << "   pack ???????? ??? ? ? ? out.csv    --> Exports data (CSV format) (default : none)" << '\n';
}

void printArgs(char* INFILE, int W, bool ROTATIONS, HEURISTIC STRATEGY, bool SHOW_PROGRESS, char* OUTFILE)
{
	std::cout << '\n' << "Solving with:" << '\n';
	std::cout << "> Input File: " << INFILE << '\n';
	std::cout << "> Width: " << W << '\n';
	std::cout << "> Rotations: " << (ROTATIONS ? "Yes" : "No") << '\n';
	std::cout << "> Sort Strategy: " << strategies[STRATEGY] << '\n';
	std::cout << "> Show Progress: " << (SHOW_PROGRESS ? "Yes" : "No") << '\n';
	std::cout << "> Output File: " << (OUTFILE ? OUTFILE : "None") << '\n';
}

void printResult(RESULT*& result)
{
	std::cout << '\n' << "Result:" << '\n';
	std::cout << "> Time Taken: " << result->time << "ms" << '\n';
	std::cout << "> Solution Height: " << result->h << '\n';
	std::cout << "> Theoretical Optimal Height: " << result->opt_h << '\n';
	std::cout << "> Ratio SOLUTION/OPTIMAL: " << float(result->h)/float(result->opt_h) << '\n';
	std::cout << "> Loss: " << result->loss << '%' << '\n';
}

std::string getFontPath(const char* exePath) {
    std::string path(exePath);
    size_t last_slash = path.find_last_of("/\\");
    if (std::string::npos != last_slash) {
        // Return the directory part + the font name
        return path.substr(0, last_slash + 1) + "anon.ttf";
    }
    // If no slash found, assume font is in the current directory
    return "anon.ttf";
}

int main(int argc, char* argv[])
{
	// Check Args
	if (argc < 3)
	{
		printUsage();
		return EXIT_FAILURE;
	}

	// Get Args
	char * EXE          = argv[0];
	char * INFILE       = argv[1];
	int W               = atoi(argv[2]);
	bool ROTATIONS      = (argc >= 4) ? (*argv[3] == '.' ? false : atoi(argv[3])) : false;
	HEURISTIC STRATEGY  = (argc >= 5) ? (*argv[4] == '.' ? HEURISTIC::descHeight : ((HEURISTIC) atoi(argv[4]))) : HEURISTIC::descHeight;
	bool VERBOSE        = (argc >= 6) ? (*argv[5] == '.' ? true : atoi(argv[5])) : true;
	char * OUTFILE      = (argc >= 7) ? (*argv[6] == '.' ? NULL : argv[6]) : NULL;

	// Print input arguments
	if (VERBOSE)
		printArgs(INFILE, W, ROTATIONS, STRATEGY, VERBOSE, OUTFILE);

	// Get rectangles from input file
	SHAPE_VEC* RECTANGLES = new SHAPE_VEC;

	// Open file
	std::ifstream ifs;
	ifs.open(INFILE, std::ifstream::in);
	if (!ifs.is_open()) // Fail check
	{
		std::cerr << '\n' << "Error: Couldnt open file " << INFILE << '\n';
		return EXIT_FAILURE;
	}
	
	// Read input file
	int id = 0;
	int w, h;
	while (ifs >> w >> h)
	{
		id++;
		RECTANGLES->push_back(new SHAPE(id, w, h));
	}
	
	// Close input file
	ifs.close();
	
	// Solve
	RESULT* result = solve(W, RECTANGLES, ROTATIONS, STRATEGY, VERBOSE);

	// Check result
	if (!result)
	{
		std::cerr << '\n' << "Error: Failed to solve..." << '\n';
		return EXIT_FAILURE;
	}
	
	printResult(result);

	// Write to output file
	if (OUTFILE)
	{
		std::ofstream ofs;
		ofs.open(OUTFILE, std::ofstream::out | std::ofstream::trunc);

		if (!ofs.is_open()) // Fail check
		{
			std::cerr << '\n' << "Error : Couldn't write to output file" << '\n';
			return EXIT_FAILURE;
		}

		ofs << "W=" << result->w << ",H=" << result->h << ",OPT(I)=" << result->opt_h << '\n';
		ofs << "SORT=" << strategies[result->sortStrategy] << ",LOSS=" << result->loss << '%' << ",ROTATIONS=" << result->rotations << '\n';
		ofs << "id,x,y,w,h" << '\n';

		// Write each line
		for (SHAPE_VEC::iterator it = result->rectangles->begin(); it != result->rectangles->end(); it++)
		{
			SHAPE* rectangle = *it;
			ofs << rectangle->id << "," << rectangle->x << "," << rectangle->y << "," << rectangle->w << "," << rectangle->h << "\n";
		}

		// Close file
		ofs.close();
	}

	// Visualize result
	visualize(result, 1280, 720, getFontPath(EXE));

    for (SHAPE* rectangle : *(result->rectangles)) {
        delete rectangle;
    }
    delete result->rectangles;
    delete result;

	return EXIT_SUCCESS;
}