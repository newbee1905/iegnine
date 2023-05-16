#include "fmt/core.h"

#include "fmt/ostream.h"
#include "inference_engine.hh"
#include "statement.hh"

#include <fstream>
#include <functional>
#include <iostream>
#include <stack>
#include <stdint.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Usage: ./inference_engine <filename>\n";
		return 1;
	}

	std::string filename = argv[1];

	std::ifstream input_file(filename);
	ie::IEngine tell_engine;
	ie::IEngine ask_engine;
	bool tell_section = false;
	bool ask_section  = false;

	for (std::string line; getline(input_file, line);) {
		if (line == "TELL") {
			tell_section = true;
			continue;
		}
		if (tell_section) {
			fmt::println("{}", line);
			tell_engine.split_expression(line);
			tell_section = false;
			continue;
		}
		if (line == "ASK") {
			ask_section = true;
			continue;
		}
		if (ask_section) {
			fmt::println("{}", line);
			ask_engine.split_expression(line);
			continue;
			ask_section = false;
		}
	}

	input_file.close();

	auto op_itr = ie::Statement::ops.begin();
	for (; op_itr != ie::Statement::ops.end(); ++op_itr) {
		/* ie::Statement::tt[*op_itr] = [&]() -> bool { return false; }; */
	}

	tell_engine.evaluate_tt();
	ask_engine.evaluate_tt();

	fmt::println("{}", ie::Statement::ops.size());
	for (std::string_view op : ie::Statement::ops) {
		fmt::print("| {} ", op);
	}
	fmt::println("| R | A |");
	int count = 0;
	for (std::uint64_t i = 0; i < (1 << ie::Statement::ops.size()); ++i) {
		std::unordered_map<std::string, bool> tt;
		op_itr = ie::Statement::ops.begin();
		for (std::int16_t j = 0; op_itr != ie::Statement::ops.end(); ++op_itr, ++j) {
			ie::Statement::tt[*op_itr]  = [&]() -> bool { return (bool)(i & (1ull << j)); };
			ie::Statement::tt2[*op_itr] = (bool)(i & (1ull << j));
			fmt::print("| {} ", (bool)((i & (1ull << j))) ? 1 : 0);
		}
		/* fmt::println("|"); */
		tell_engine.evaluate_tt2();
		ask_engine.evaluate_tt2();
		fmt::print("| {} ", ie::Statement::tt2[tell_engine.final_atomic()] ? 1 : 0);
		fmt::println("| {} |", ie::Statement::tt2[ask_engine.final_atomic()] ? 1 : 0);
		/* fmt::print("| {} ", ie::Statement::tt[tell_engine.final_atomic()]() ? 1 : 0); */
		/* fmt::println("| {} |", ie::Statement::tt[ask_engine.final_atomic()]() ? 1 : 0); */
		/* fmt::println("{} => {}", tell_engine.final_atomic(),
		 * ie::Statement::tt2[tell_engine.final_atomic()]); */
		/* fmt::println("{} => {}", ask_engine.final_atomic(),
		 * ie::Statement::tt2[ask_engine.final_atomic()]); */
		/* bool res = ie::Statement::tt2[tell_engine.final_atomic()] &&
		 * ie::Statement::tt2[ask_engine.final_atomic()]; */
		/* if (!res) */
		/* 	i +=  */
		/* else */
		/* 	count++; */
		/* count += ie::Statement::tt[tell_engine.final_atomic()]() && */
		/*          ie::Statement::tt[ask_engine.final_atomic()](); */
		count += ie::Statement::tt2[tell_engine.final_atomic()] &&
		         ie::Statement::tt2[ask_engine.final_atomic()];
		tell_engine.final_atomic() = "";
		ask_engine.final_atomic()  = "";
	}
	/* fmt::println("{}\n{}", tell_engine.final_atomic(), ask_engine.final_atomic()); */
	if (count > 0)
		fmt::println("YES: {}", count);
	else
		fmt::println("NO");
}
