#include <string>
#include <sstream>
#include <algorithm>
#include <regex>
#include "StringUtils.h"

void HexToRGB(const std::string& hex, int& r, int& g, int& b) {
    std::stringstream ss;
    ss << std::hex << hex.substr(0, 2);
    ss >> r;
    ss.clear();
    ss << std::hex << hex.substr(2, 2);
    ss >> g;
    ss.clear();
    ss << std::hex << hex.substr(4, 2);
    ss >> b;
}
void BlendColor(int r1, int g1, int b1, int r2, int g2, int b2, float ratio, int& r, int& g, int& b) {
    r = static_cast<int>(r1 + ratio * (r2 - r1));
    g = static_cast<int>(g1 + ratio * (g2 - g1));
    b = static_cast<int>(b1 + ratio * (b2 - b1));
}
namespace String {
    std::string ToLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return std::tolower(c);
            });
        return result;
    }
    std::string ToUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return std::toupper(c);
            });
        return result;
    }
    std::string Trim(const std::string& str) {
        size_t start = str.find_first_not_of(' ');
        size_t end = str.find_last_not_of(' ');
        if (start == std::string::npos || end == std::string::npos)
            return "";
        return str.substr(start, end - start + 1);
    }
    std::string ToASCII(const std::string& input, const bool AllowNull) {
        std::string result;
        for (char ch : input) {
            if (ch >= !AllowNull && ch <= 127) {
                result += ch;
            }
        }
        return result;
    }
    std::string RemoveANSI(const std::string& input) {
        std::regex ansi_escape("\033\\[[0-9;]*[mK]");
        return std::regex_replace(input, ansi_escape, "");
    }
    std::string RemoveWhitespace(const std::string& str) {
        std::string result = str;
        result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
        return result;
    }
    bool EqualsIgnoreCase(const std::string& obj1, const std::string& obj2) {
        return ToLower(obj1) == ToLower(obj2);
    }
    bool EqualsIgnoreWhitespace(const std::string& obj1, const std::string& obj2) {
        return RemoveWhitespace(obj1) == RemoveWhitespace(obj2);
    }
    bool EqualsIgnoreSpaceAndCase(const std::string& obj1, const std::string& obj2) {
        return RemoveWhitespace(ToLower(obj1)) == RemoveWhitespace(ToLower(obj2));
    }
    bool Equals(const std::string& obj1, const std::string& obj2, ComparisonType T) {
        switch (T) {
        case ComparisonType::Equals:
            return obj1 == obj2;
        case ComparisonType::IgnoreCase:
            return EqualsIgnoreCase(obj1, obj2);
        case ComparisonType::IgnoreWhitespace:
            return EqualsIgnoreWhitespace(obj1, obj2);
        case ComparisonType::IgnoreSpaceAndCase:
            return EqualsIgnoreSpaceAndCase(obj1, obj2);
        default:
            return false;
        }
    }
    std::string Colorize(const std::string& text, const std::string& hexFrom, const std::string& hexTo) {
        int r1, g1, b1, r2, g2, b2;
        HexToRGB(hexFrom, r1, g1, b1);
        if (hexTo.empty())
            HexToRGB(hexFrom, r2, g2, b2);
        else
            HexToRGB(hexTo, r2, g2, b2);

        int length = 0;
        for (size_t i = 0; i < text.length(); ++i) {
            if ((text[i] & 0xC0) != 0x80) {
                ++length;
            }
        }

        std::string coloredText;
        int charIndex = 0;
        for (size_t i = 0; i < text.length();) {
            float ratio = static_cast<float>(charIndex) / static_cast<float>(length - 1);
            int r, g, b;
            BlendColor(r1, g1, b1, r2, g2, b2, ratio, r, g, b);

            std::stringstream ss;
            ss << "\033[38;2;" << r << ";" << g << ";" << b << "m";

            if ((text[i] & 0x80) == 0) {
                ss << text[i];
                ++i;
            }
            else if ((text[i] & 0xE0) == 0xC0) {
                ss << text[i] << text[i + 1];
                i += 2;
            }
            else if ((text[i] & 0xF0) == 0xE0) {
                ss << text[i] << text[i + 1] << text[i + 2];
                i += 3;
            }
            else if ((text[i] & 0xF8) == 0xF0) {
                ss << text[i] << text[i + 1] << text[i + 2] << text[i + 3];
                i += 4;
            }
            coloredText += ss.str();
            ++charIndex;
        }

        coloredText += "\033[0m";
        return coloredText;
    }
}