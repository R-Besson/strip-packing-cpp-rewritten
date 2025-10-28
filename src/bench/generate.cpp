/**====================================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Optimal Instance generator interface
 *=====================================================**/

#include <iostream>
#include <fstream>
#include <random>
#include "instance_gen.hpp"

/**=======================
 *  User Interface
 *========================**/

void printUsage()
{
	std::cout << '\n' << "Usage:" << '\n';
	std::cout << "   <exe> <width> <rectangle_count> <ratio height/width> <output_file>" << '\n' << '\n';
	std::cout << "Examples:" << '\n';
	std::cout << "   gen 100                  --> Width 100" << '\n';
	std::cout << "   gen ??? 10               --> 10 rectangles" << '\n';
	std::cout << "   gen ??? ?? 0.5           --> 1/2 ratio = 2 times as wide" << '\n';
	std::cout << "   gen ??? ?? ??? out.csv   --> Exports rectangles (default : none)" << '\n';
}

void printArgs(int W, int N, float RATIO, char* OUTFILE)
{
	std::cout << '\n' << "Generating with:" << '\n';
	std::cout << "> Width: " << W << '\n';
	std::cout << "> Rectangle Count: " << N << '\n';
	std::cout << "> Ratio Height/Width: " << RATIO << '\n';
	std::cout << "> Output File: " << (OUTFILE ? OUTFILE : "None") << '\n';
}

int main(int argc, char* argv[])
{
	// Check Argument Validity
	if (argc < 5)
	{
		printUsage();
		return EXIT_FAILURE;
	}

	// Get args
	int W           = atoi(argv[1]);
	int N           = atoi(argv[2]);
	float RATIO     = atof(argv[3]);
	char* OUTFILE   = (argc >= 5) ? (*argv[4] == '.' ? NULL : argv[4]) : NULL;

	// Print args
	printArgs(W, N, RATIO, OUTFILE);

	// Gen instance
	SHAPE_VEC* rectangles = genInstance(W, N, RATIO);

	// File setup
	std::ofstream ofs;
	ofs.open(OUTFILE, std::ofstream::out | std::ofstream::trunc);

	if (!ofs.is_open()) // Fail check
	{
		std::cerr << '\n' << "Error: Couldn't write to output file" << '\n';
		return EXIT_FAILURE;
	}

	// Export data
	for (SHAPE* rectangle : *rectangles)
	{
		ofs << rectangle->w << ' ' << rectangle->h << '\n';
	}

	// Close file
	ofs.close();

	std::cout << '\n' << "Successfully exported data to '" << OUTFILE << "'\n";

	for (SHAPE* rectangle : *rectangles) {
        delete rectangle;
    }
    delete rectangles;

	return EXIT_SUCCESS;
}
