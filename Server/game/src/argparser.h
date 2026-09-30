#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

inline std::vector<std::string> SplitArguments(const char* argument) {
	std::istringstream iss(argument);
	std::vector<std::string> args;
	std::string token;

	while (iss >> token)
		args.emplace_back(std::move(token));

	return args;
}

inline bool IsNumber(const std::string& s) {
	return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

inline bool TryParseInt(const std::string& s, int& out) {
	try {
		out = std::stoi(s);
		return true;
	} catch (...) {
		return false;
	}
}