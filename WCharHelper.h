#pragma once
#include <string>

class WCharHelper {
public:
    static std::string WStringToString(const std::wstring& wstr);
    static std::wstring StringToWString(const std::string& str);
    static std::string WCharToString(const wchar_t* wcharStr);
    static std::wstring CharToWString(const char* charStr);
    static const char* WStringToConstChar(const std::wstring& wstr);
    static const wchar_t* StringToConstWChar(const std::string& str);
};