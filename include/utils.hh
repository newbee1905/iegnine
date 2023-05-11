/// This file is for some ultility functions
/// They will be put inside namespace ie which
/// is a shortcut stands for my nickname

#ifndef UTILS_HH
#define UTILS_HH

#include "fmt/core.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cwchar>
#include <exception>
#include <fstream>
#include <functional>
#include <iterator>
#include <string_view>

// TODO may consider split the ultils.hh file into many
// smaller file
namespace ie {

std::string_view tolower_string(std::string_view str);

/// No constructor custom struct
/// Should have the best constructor possible
/// from the compiler side
///
/// The ie::pair should be similar to std::pair
/// after optimisation in Clang and GCC
///
/// However, for msvc, this version has less
/// lines of asm code
template <typename T1, typename T2> struct pair {
	T1 fst;
	T2 sec;
};

template <typename T1, typename T2>
inline bool operator==(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
	return lhs.fst == rhs.fst && lhs.sec == rhs.sec;
}

template <typename T1, typename T2>
inline bool operator!=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
	return lhs.fst != rhs.fst || lhs.sec != rhs.sec;
}

template <typename T1, typename T2>
inline bool operator<(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
	if (lhs.fst < rhs.fst)
		return true;
	if (lhs.fst == rhs.fst && lhs.sec < rhs.sec)
		return true;
	return false;
}

template <typename T1, typename T2>
inline bool operator>(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
	return rhs < lhs;
}

/// get the number of element inside __VA_ARGS__
/// at compile time
template <typename... Args> constexpr std::size_t va_count(Args &&...) { return sizeof...(Args); }

/// split string at compile time
/// return an array of element
template <std::size_t N>
constexpr std::array<std::string_view, N> split(std::string_view str, char delim = ',') {
	std::array<std::string_view, N> arr{};
	std::size_t start = 0, end = 0;

	for (std::size_t i{}; i < N && end != std::string_view::npos; ++i) {
		end    = str.find_first_of(delim, start);
		arr[i] = str.substr(start, end - start);
		start  = end + 2; // remove the space after the ','
	}

	return arr;
}

/// split string at compile time
/// return an array of enum with
/// its string and index value
template <std::size_t N, typename E>
constexpr std::array<ie::pair<std::string_view, E>, N> split_enum(std::string_view str,
                                                                  char delim = ',') {
	std::array<ie::pair<std::string_view, E>, N> arr{};
	std::size_t start = 0, end = 0;

	for (std::size_t i{}; i < N && end != std::string_view::npos; ++i) {
		end    = str.find_first_of(delim, start);
		arr[i] = ie::pair<std::string_view, E>{
				str.substr(start, end - start),
				E(i),
		};
		start = end + 2; // remove the space after the ','
	}

	return arr;
}

/// Map built at compile time
/// Linear search instead of log N
/// Will be optimised by compiler and more suitable
/// for small map
template <typename key, typename val, std::size_t size> struct map {
	std::array<ie::pair<key, val>, size> data;

	[[nodiscard]] constexpr val at(const key &k) const {
		const auto itr =
				std::find_if(std::begin(data), std::end(data), [&k](const auto &v) { return v.fst == k; });
		if (itr != end(data))
			return itr->sec;
		else {
			throw std::range_error(fmt::format("`{}` Not Found", k));
		}
	}

	[[nodiscard]] constexpr val operator[](const key &k) const { return at(k); }
};

}; // namespace ie

#endif // !UTILS_HH
