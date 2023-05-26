#include "fmt/core.h"

#include "bc_iengine.hh"
#include "fc_iengine.hh"
#include "fmt/ostream.h"
#include "iengine.hh"
#include "tt_iengine.hh"
/* #include "inference_engine.hh" */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cout << "Usage: ./inference_engine <mode> <filename>\n";
		return 1;
	}

	++argv;

	std::string algo     = *argv++;
	std::string filename = *argv++;

	std::unique_ptr<ie::IEngine> e = nullptr;
	bool optimal_cnf               = false;
	bool dpll                      = false;

	for (; *argv; ++argv) {
		if (std::string(*argv) == "--cnf")
			optimal_cnf = true;
		else if (std::string(*argv) == "--dpll")
			dpll = true;
	}

	switch (algo[0]) {
	case 'F':
		e = std::make_unique<ie::FCIEngine>();
		/* fmt::println("FC:"); */
		break;
	case 'B':
		e = std::make_unique<ie::BCIEngine>();
		/* fmt::println("BC:"); */
		break;
	case 'T':
		e = std::make_unique<ie::TTIEngine>(optimal_cnf, dpll);
		break;
	default:
		fmt::println(stderr, "This method does not exist or not supported yet.");
		return 1;
	}

	std::ifstream input_file(filename);
	std::string kb_str;
	std::string query_str;

	int mode = 0;
	for (std::string line; getline(input_file, line);) {
		if (line == "TELL" || line == "ASK") {
			mode++;
			continue;
		}
		/* fmt::println("{}", line); */
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
		fmt::print("YES: ");
		e->result_output();
		return 0;
	}

	fmt::println("NO");
	// e->result_output();

	return 0;
}
