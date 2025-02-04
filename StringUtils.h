#pragma once
#include <string>
enum class ComparisonType {
	Equals,
	IgnoreCase,
	IgnoreWhitespace,
	IgnoreSpaceAndCase
};
namespace String {
	std::string ToLower(const std::string& str);
	std::string ToUpper(const std::string& str);
	std::string Trim(const std::string& str);
	std::string ToASCII(const std::string& input, const bool AllowNull = false);
	std::string RemoveANSI(const std::string& input);
	std::string RemoveWhitespace(const std::string& str);
	bool EqualsIgnoreCase(const std::string& obj1, const std::string& obj2);
	bool EqualsIgnoreWhitespace(const std::string& obj1, const std::string& obj2);
	bool EqualsIgnoreSpaceAndCase(const std::string& obj1, const std::string& obj2);
	bool Equals(const std::string& obj1, const std::string& obj2, ComparisonType T = ComparisonType::Equals);
	std::string Colorize(const std::string& text, const std::string& hexFrom, const std::string& hexTo = "");
}