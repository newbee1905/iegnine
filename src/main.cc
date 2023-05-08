#include "fmt/core.h"

#include "fmt/ostream.h"
#include "inference_engine.hh"

#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Usage: ./inference_engine <filename>\n";
		return 1;
	}

	std::string filename = argv[1];

	std::ifstream input_file(filename);
	std::vector<std::string> tokens;
	bool ask_section = false;

	for (std::string line; getline(input_file, line);) {
		if (line == "TELL") {
			ask_section = true;
		} else if (ask_section) {
			fmt::println("{}", line);
			tokens = ie::split_expression(line);
			break;
		}
	}

	input_file.close();

	for (const auto &token : tokens) {
		if (ie::is_operator(token[0])) {
			fmt::println("Opeartor: {}", token);
		} else {
			fmt::println("Identifier: {}", token);
		}
	}
	return 0;
}
