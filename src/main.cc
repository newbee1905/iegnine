#include "fmt/core.h"

#include "BCIEngine.hh"
#include "FCIEngine.hh"
#include "fmt/ostream.h"
#include "iengine.hh"
/* #include "inference_engine.hh" */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cout << "Usage: ./inference_engine <filename> <mode>\n";
		return 1;
	}

	std::unique_ptr<ie::IEngine> e = nullptr;

	switch (argv[2][0]) {
	case 'F':
		e = std::make_unique<ie::FCIEngine>();
		fmt::println("FC:");
		break;
	case 'B':
		e = std::make_unique<ie::BCIEngine>();
		fmt::println("BC:");
		break;
	default:
		fmt::println(stderr, "This method does not exist or not supported yet.");
		return 1;
	}

	std::ifstream input_file(argv[1]);
	std::string kb_str;
	std::string query_str;

	int mode = 0;
	for (std::string line; getline(input_file, line);) {
		if (line == "TELL" || line == "ASK") {
			mode++;
			continue;
		}
		fmt::println("{}", line);
		switch (mode) {
		case 1:
			e->kb_str() = line;
			break;
		case 2:
			e->query_str() = line;
			break;
		}
	}

	input_file.close();

	bool result = e->solve();

	if (result) {
		fmt::println("YES");
		e->result_output();
		return 0;
	}

	fmt::println("NO");
	//e->result_output();

	return 0;
}
