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

	ie::IEngine tell_engine;
	ie::IEngine ask_engine;

	std::ifstream input_file(filename);
	std::vector<std::string> tell_tokens;

	int mode = 0;
	for (std::string line; getline(input_file, line);) {
		if (line == "TELL" || line == "ASK") {
			mode++;
			continue;
		}
		fmt::println("{}", line);
		switch (mode) {
		case 1:
			tell_engine.split_expression(line);
			break;
		case 2:
			ask_engine.split_expression(line);
			break;
		}
	}

	input_file.close();

	return 0;
}
