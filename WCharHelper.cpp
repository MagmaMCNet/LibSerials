#define _CRT_SECURE_NO_WARNINGS
#include "WCharHelper.h"
#include <locale>
#include <string>

std::string WCharHelper::WStringToString(const std::wstring& wstr) {
    std::string result;
    std::mbstate_t state{};
    const wchar_t* src = wstr.data();
    size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    if (len != static_cast<size_t>(-1)) {
        result.resize(len);
        std::wcsrtombs(result.data(), &src, len, &state);
    }
    return result;
}

std::wstring WCharHelper::StringToWString(const std::string& str) {
    std::wstring result;
    std::mbstate_t state{};
    const char* src = str.data();
    size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
    if (len != static_cast<size_t>(-1)) {
        result.resize(len);
        std::mbsrtowcs(result.data(), &src, len, &state);
    }
    return result;
}

std::string WCharHelper::WCharToString(const wchar_t* wcharStr) {
    if (!wcharStr) return "";
    return WStringToString(std::wstring(wcharStr));
}

std::wstring WCharHelper::CharToWString(const char* charStr) {
    if (!charStr) return L"";
    return StringToWString(std::string(charStr));
}

const char* WCharHelper::WStringToConstChar(const std::wstring& wstr) {
    static thread_local std::string convertedStr;
    convertedStr = WStringToString(wstr);
    return convertedStr.c_str();
}

const wchar_t* WCharHelper::StringToConstWChar(const std::string& str) {
    static thread_local std::wstring convertedWStr;
    convertedWStr = StringToWString(str);
    return convertedWStr.c_str();
}
