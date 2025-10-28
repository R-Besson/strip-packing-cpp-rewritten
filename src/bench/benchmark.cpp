/**====================================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Random Instance Benchmarking
 *=====================================================**/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <random>

#include "instance_gen.hpp"           // 2D SPP Instance Generator
#include "../packer/packer.h"       // 2D Packing Library

#define W 1000

void printUsage()
{
	std::cout << '\n' << "Usage (* = optional, use a '.' for default value):" << '\n';
	std::cout << "   <exe> <iteration_count> <rectangle_count> <ratio height/width> <output_file*> <verbose*>" << '\n' << '\n';
	std::cout << "Examples:" << '\n';
	std::cout << "   bench 10                       --> 10 iterations" << '\n';
	std::cout << "   bench ?? 10                    --> 10 rectangles" << '\n';
	std::cout << "   bench ?? ?? 0.5                --> 1/2 ratio = 2 times as wide" << '\n';
	std::cout << "   bench ?? ?? ??? out.csv        --> Exports data (CSV format) (default : none)" << '\n';
	std::cout << "   bench ?? ?? ??? ??????? 1      --> Verbose mode (default : off)" << '\n';
}

void printArgs(int IT, int N, float RATIO, char* OUTFILE, bool VERBOSE)
{
	std::cout << '\n' << "Benching with:" << '\n';
	std::cout << "> Iteration Count: " << IT << '\n';
	std::cout << "> Rectangle Count: " << N << '\n';
	std::cout << "> Ratio Height/Width: " << RATIO << '\n';
	std::cout << "> Output File: " << (OUTFILE ? OUTFILE : "None") << '\n';
	std::cout << "> Verbose: " << (VERBOSE ? "Yes" : "No") << '\n' << '\n';
}

double keepDigits(double f, double d)
{
	double precision = pow(10.0, d);
	return std::round(f*precision)/precision;
}

int main(int argc, char* argv[])
{
	// Check Argument Validity
	if (argc < 4)
	{
		printUsage();
		return EXIT_FAILURE;
	}

	// Get+Print args
	int IT          = atoi(argv[1]);
	int N           = atoi(argv[2]);
	float RATIO     = atof(argv[3]);
	char* OUTFILE   = (argc >= 5) ? (*argv[4] == '.' ? NULL : argv[4]) : NULL;
	bool VERBOSE    = (argc >= 6) ? (*argv[5] == '.' ? false : atoi(argv[5])) : false;
	printArgs(IT, N, RATIO, OUTFILE, VERBOSE);

	// File setup
	std::ofstream ofs;
	if (OUTFILE)
	{
		ofs.open(OUTFILE, std::ofstream::out | std::ofstream::trunc);

		if (!ofs.is_open()) // Fail check
		{
			std::cerr << '\n' << "Error: Couldn't write to output file" << '\n';
			return EXIT_FAILURE;
		}
	}

	// CSV setup
	if (OUTFILE)
		ofs << "#,H,OPT_H,H/OPT_H" << '\n';
	
	// Iteration
	double  best = INFINITY,
			worst = -INFINITY,
			sum = 0;
	
	if (VERBOSE)
		std::cout << "Iterations:" << '\n';
	for (int i = 1; i <= IT; i++)
	{
		SHAPE_VEC* rectangles = genInstance(W, N, RATIO);
		RESULT* result = solve(W, rectangles, false, HEURISTIC::descHeight, false);

		// Alpha = A(I)/OPT(I)
		double alpha = keepDigits(double(result->h)/double(float(W)*RATIO), 4.0);

		best = std::min(best, alpha);
		worst = std::max(worst, alpha);
		sum += alpha;

		if (VERBOSE)
			std::cout << "IT=" << i << "/" << IT << ", worst=" << worst << ", best=" << best << '\n';
		if (OUTFILE)
			ofs << i << ',' << result->h << ',' << (int(float(W)*RATIO)) << ',' << alpha << '\n';

		// Cleanup
		for (auto shape_ptr : *rectangles) // C++11 range-based for loop
		{
			delete shape_ptr; // Delete the individual shape object
		}
		delete rectangles;
		delete result;
	}
	double average = sum/IT;

	// Print/Output results
	if (VERBOSE)
		std::cout << '\n';
	std::cout << "DONE! worst=" << worst << ", best=" << best << ", avg=" << average << '\n';
	if (OUTFILE)
		ofs << "worst=" << worst << ",best=" << best << ",avg=" << average << '\n';
	
	// Close file
	ofs.close();

	return EXIT_SUCCESS;
}
