#include "fmt/core.h"

#include "fmt/ostream.h"
#include "inference_engine.hh"
#include "FCIEngine.hh"

#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Usage: ./inference_engine <filename>\n";
		return 1;
	}

	std::string filename = argv[1];


	//FC
	ie::FCIEngine engine;
	engine.solve(filename);

	return 0;
}
