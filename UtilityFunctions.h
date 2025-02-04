#pragma once
#include <string>
#include <cwchar>
#include <cstring>
#include <locale>

class WCharHelper {
public:
    static inline std::string WStringToString(const std::wstring& wstr) {
        if (wstr.empty()) return {};

        std::string result;
        std::mbstate_t state{};
        const wchar_t* src = wstr.c_str();
        size_t length = 0;

        if (wcsrtombs_s(&length, nullptr, 0, &src, 0, &state) != 0 || length == 0)
            return {};

        result.resize(length);
        wcsrtombs_s(&length, result.data(), result.size(), &src, result.size(), &state);
        return result;
    }

    static inline std::wstring StringToWString(const std::string& str) {
        if (str.empty()) return {};

        std::wstring result;
        std::mbstate_t state{};
        const char* src = str.c_str();
        size_t length = 0;

        if (mbsrtowcs_s(&length, nullptr, 0, &src, 0, &state) != 0 || length == 0)
            return {};

        result.resize(length);
        mbsrtowcs_s(&length, result.data(), result.size(), &src, result.size(), &state);
        return result;
    }

    static inline std::string WCharToString(const wchar_t* wcharStr) {
        if (!wcharStr) return {};
        return WStringToString(std::wstring(wcharStr));
    }

    static inline std::wstring CharToWString(const char* charStr) {
        if (!charStr) return {};
        return StringToWString(std::string(charStr));
    }

    static inline const char* WStringToConstChar(const std::wstring& wstr) {
        thread_local std::string convertedStr = WStringToString(wstr);
        return convertedStr.c_str();
    }

    static inline const wchar_t* StringToConstWChar(const std::string& str) {
        thread_local std::wstring convertedWStr = StringToWString(str);
        return convertedWStr.c_str();
    }
};
