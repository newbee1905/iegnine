#ifndef IENGINE_HH
#define IENGINE_HH

#include <functional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utils.hh"

namespace ie {
const std::regex TT_REGEX("([&~)(]|[;\\n]|[|=][|>]|<=>)|\\w+");
const std::regex HORN_REGEX("(&|;|=>)|\\w+");

class IEngine {
protected:
	bool isYes;
	std::string m_kb_str, m_query_str;
	std::unordered_map<std::string, std::vector<std::vector<std::string>>> m_kb;
	std::vector<std::string> m_inferred_facts;

public:
	IEngine() {}
	// setter + getter
	std::string &kb_str() { return m_kb_str; }
	std::string &query_str() { return m_query_str; }

public:
	static std::vector<std::string> split_string(std::string s, const char delim);
	static void split_string(std::vector<std::string> out, std::string s, const char delim);
	virtual bool parse()            = 0;
	virtual bool solve()            = 0;
	virtual bool entailment_check() = 0;
	virtual void result_output()    = 0;
};

class HornIEngine : public IEngine {};
} // namespace ie

#endif // IENGINE_HH
