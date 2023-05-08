#ifndef ENUM_STR_HH
#define ENUM_STR_HH

#include "fmt/format.h"
#include "fmt/ostream.h"

#include <array>
#include <string_view>

#include "utils.hh"

/// Auto generate an array of string for enum value
/// at compile time with constexpr and string_view
///
/// FIX: Doesn't work with enum that has custom value
#define ENUM_STR(ENUM_NAME, ...)                                                                   \
	enum ENUM_NAME { __VA_ARGS__ };                                                                  \
	constexpr std::array<std::string_view, ie::va_count(__VA_ARGS__)> __tag_##ENUM_NAME =            \
			ie::split<ie::va_count(__VA_ARGS__)>(#__VA_ARGS__);                                          \
	constexpr std::string_view GetString##ENUM_NAME(ENUM_NAME index) {                               \
		return __tag_##ENUM_NAME[index];                                                               \
	}                                                                                                \
                                                                                                   \
	constexpr std::size_t GetLengthEnum##ENUM_NAME() { return ie::va_count(__VA_ARGS__); }           \
                                                                                                   \
	template <> struct fmt::formatter<ENUM_NAME> {                                                   \
		template <typename ParseContext> constexpr auto parse(ParseContext &ctx);                      \
		template <typename FormatContext> auto format(ENUM_NAME const &e, FormatContext &ctx);         \
	};                                                                                               \
                                                                                                   \
	template <typename ParseContext>                                                                 \
	constexpr auto fmt::formatter<ENUM_NAME>::parse(ParseContext &ctx) {                             \
		return ctx.begin();                                                                            \
	}                                                                                                \
                                                                                                   \
	template <typename FormatContext>                                                                \
	auto fmt::formatter<ENUM_NAME>::format(ENUM_NAME const &e, FormatContext &ctx) {                 \
		return fmt::format_to(ctx.out(), "{}", GetString##ENUM_NAME(e));                               \
	}

/// Auto generate a constexpr map of enum value
/// as in string_view and as enum value
///
/// FIX: Doesn't work with enum that has custom value
#define ENUM_MAP(ENUM_NAME, ...)                                                                   \
	enum ENUM_NAME { __VA_ARGS__ };                                                                  \
	constexpr std::array<ie::pair<std::string_view, ENUM_NAME>, ie::va_count(__VA_ARGS__)>           \
			__tag_map_##ENUM_NAME = ie::split_enum<ie::va_count(__VA_ARGS__), ENUM_NAME>(#__VA_ARGS__);  \
	static constexpr auto __map_##ENUM_NAME =                                                        \
			ie::map<std::string_view, ENUM_NAME, ie::va_count(__VA_ARGS__)>{{__tag_map_##ENUM_NAME}};    \
	constexpr std::string_view GetString##ENUM_NAME(ENUM_NAME index) {                               \
		return __map_##ENUM_NAME.data[index].fst;                                                      \
	}                                                                                                \
	constexpr ENUM_NAME GetEnum##ENUM_NAME(const std::string_view k) {                               \
		return __map_##ENUM_NAME[k];                                                                   \
	}                                                                                                \
                                                                                                   \
	constexpr std::size_t GetLengthEnum##ENUM_NAME() { return ie::va_count(__VA_ARGS__); }           \
                                                                                                   \
	template <> struct fmt::formatter<ENUM_NAME> {                                                   \
		template <typename ParseContext> constexpr auto parse(ParseContext &ctx);                      \
		template <typename FormatContext> auto format(ENUM_NAME const &e, FormatContext &ctx);         \
	};                                                                                               \
                                                                                                   \
	template <typename ParseContext>                                                                 \
	constexpr auto fmt::formatter<ENUM_NAME>::parse(ParseContext &ctx) {                             \
		return ctx.begin();                                                                            \
	}                                                                                                \
                                                                                                   \
	template <typename FormatContext>                                                                \
	auto fmt::formatter<ENUM_NAME>::format(ENUM_NAME const &e, FormatContext &ctx) {                 \
		return fmt::format_to(ctx.out(), "{}", GetString##ENUM_NAME(e));                               \
	}

#endif // !ENUM_STR_HH
