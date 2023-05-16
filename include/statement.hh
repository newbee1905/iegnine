#ifndef STATEMENT_HH
#define STATEMENT_HH

#include <functional>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ie {

struct Statement {
	static std::map<std::string, std::function<bool()>> tt;
	static std::map<std::string, bool> tt2;
	/* static std::function<bool()> tt_get(const Statement &s); */
	/* static std::function<bool()> tt_get(Statement *s); */
	static void gen_tt();

	static std::set<std::string> ops;

	std::string statement;

	Statement(std::string s);

	const std::string &operator*();
	const std::string &operator*() const;
	/* std::function<bool()> operator!(); */
	/* std::function<bool()> operator&(const Statement &b); */
	/* std::function<bool()> operator|(const Statement &b); */
	/* // => */
	/* std::function<bool()> operator>(const Statement &b); */
	/* // <=> */
	/* std::function<bool()> operator>>(const Statement &b); */
};

Statement combine_name(std::string right, std::string op);
Statement combine_name(std::string left, std::string right, std::string op);

}; // namespace ie

#endif
