#include "statement.hh"
#include "fmt/ostream.h"
#include "inference_engine.hh"

#include <functional>
#include <stdint.h>
#include <tuple>

namespace ie {
Statement::Statement(std::string s) {
	statement = s;
	if (s[0] != '(' && !IEngine::is_operator(s[0]) && !IEngine::is_separator(s[0]))
		ops.insert(std::string(s));
}

std::map<std::string, std::function<bool()>> Statement::tt{};
std::map<std::string, bool> Statement::tt2{};
std::set<std::string> Statement::ops{};

#define L(v)                                                                                       \
	[&]() -> bool {                                                                                  \
		fmt::println("Return {}", (v));                                                                \
		return (v);                                                                                    \
	}

/* std::function<bool()> Statement::tt_get(const Statement &s) { return Statement::tt[*s]; } */
/* std::function<bool()> Statement::tt_get(Statement *s) { return Statement::tt[**s]; } */

void Statement::gen_tt() {
	/* for (std::uint64_t i = 0; i < (ops.size() << 1); ++i) { */
	/* 	auto op_itr = ops.begin(); */
	/* 	for (int j = 0; op_itr != ops.end(); ++op_itr) { */
	/* 		tt[*op_itr] = L(i & (1 << j)); */
	/* 	} */
	/* } */
}

Statement combine_name(std::string right, std::string op) {
	return Statement{fmt::format("{}({})", op, right)};
}
Statement combine_name(std::string left, std::string right, std::string op) {
	return Statement{fmt::format("({}){}({})", left, op, right)};
}

const std::string &Statement::operator*() { return statement; }
const std::string &Statement::operator*() const { return statement; }

/* std::function<bool()> Statement::operator!() { return L(!(tt_get(*this)())); } */
/* std::function<bool()> Statement::operator&(const Statement &b) { */
/* 	fmt::println("Calling {} & {}", **this, *b); */
/* 	bool right = false; */
/* 	bool left  = false; */
/* 	try { */
/* 		right = (tt_get(*this))() && (tt_get(b))(); */
/* 	} catch (const std::bad_function_call &e) { */
/* 		fmt::print(stderr, "Right Statement Error: {}\n", **this); */
/* 	} */
/* 	try { */
/* 		left = (tt_get(b))(); */
/* 	} catch (const std::bad_function_call &e) { */
/* 		fmt::print(stderr, "Left Statement Error: {}\n", *b); */
/* 	} */
/* 	return L(right && left); */
/* } */
/* std::function<bool()> Statement::operator|(const Statement &b) { */
/* 	fmt::println("Calling {} || {}", **this, *b); */
/* 	return L((tt_get(*this))() || (tt_get(b))()); */
/* } */
/* std::function<bool()> Statement::operator>(const Statement &b) { */
/* 	fmt::println("Calling {} => {}", **this, *b); */
/* 	return L(!((tt_get(*this))()) || (tt_get(b))()); */
/* } */
/* std::function<bool()> Statement::operator>>(const Statement &b) { */
/* 	fmt::println("Calling {} <=> {}", **this, *b); */
/* 	return L(((tt_get(*this))() && (tt_get(b))()) || ((!(tt_get(*this))()) && !((tt_get(b))()))); */
/* } */

#undef L
}; // namespace ie
