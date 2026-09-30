#ifndef _INCLUDE_HEADER_COMMON_UTILS_
#define _INCLUDE_HEADER_COMMON_UTILS_
#include <string>
#include <charconv>
#include <type_traits>
#include <optional>
#include <cstring>
#include <limits>
#include <msl/msl.h>
// Generic str_to_number using std::from_chars for integral types
template <typename T>
	requires std::is_integral_v<T>
inline std::optional<T> str_to_number(const char* in) {
	if (!in || *in == '\0') return std::nullopt;

	T value{};
	auto [ptr, ec] = std::from_chars(in, in + std::strlen(in), value);
	if (ec != std::errc()) return std::nullopt;

	return value;
}

// Fallback for float, double, long double using std::strtoX
template <typename T>
	requires std::is_floating_point_v<T>
inline std::optional<T> str_to_number(const char* in) {
	if (!in || *in == '\0') return std::nullopt;

	char* end{};
	T value = static_cast<T>(std::strtod(in, &end));
	if (in == end) return std::nullopt;

	return value;
}

// Helper for in-place output, same behavior as original (legacy compatible)
template <typename T>
inline bool str_to_number(T& out, const char* in) {
	auto opt = str_to_number<T>(in);
	if (!opt) return false;
	out = *opt;
	return true;
}

// Specialization for bool
template <>
inline std::optional<bool> str_to_number<bool>(const char* in) {
	if (!in || *in == '\0') return std::nullopt;

	// Accept only "0" or "1"
	if (std::strcmp(in, "1") == 0) return true;
	if (std::strcmp(in, "0") == 0) return false;

	return std::nullopt;
}

// STRING UTILITIES
inline bool str_to_string(std::string& dest, const char* from) {
	if (!from) return false;
	dest.assign(from, strnlen(from, 4096)); // safely bounded
	return true;
}

template <size_t N>
inline bool str_to_cstring(char(&array)[N], const char* from) {
	if (!from) return false;
	std::strncpy(array, from, N - 1);
	array[N - 1] = '\0'; // always null-terminate
	return true;
}

// ENUM SUPPORT
template <typename Enum>
	requires std::is_enum_v<Enum>
inline bool str_to_enum(Enum& out, const char* in) {
	using Underlying = std::underlying_type_t<Enum>;
	Underlying value{};
	if (!str_to_number(value, in)) return false;
	out = static_cast<Enum>(value);
	return true;
}

#endif // _INCLUDE_HEADER_COMMON_UTILS_
