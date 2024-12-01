#ifndef BLIB_H
#define BLIB_H

#include <string>
#include <tchar.h>

uint32_t crc32(const std::string& data);
std::string crc32ToHex(uint32_t crc);

void EnableANSIColors(bool value);
void ClearLastLines(int num_lines);
void MoveCursorDown(int lines);
std::string CleanString(const std::string& str);

std::wstring stringToWString(const std::string& str);
std::string GetHWID();
std::string to_lower(const std::string& str);
std::string trimSpaces(const std::string& str);
void HexToRGB(const std::string& hex, int& r, int& g, int& b);
std::string Color_Text(const std::string& text, const std::string& hexFrom, const std::string& hexTo);

std::string TCHARToString(const TCHAR* tcharStr);
bool IsProgramRunning(const std::string& programName);
void EndProcess(const std::string& programName);

std::string performHTTPRequest(const std::string& url, const std::string& requestBody);
size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
void Add_WindowsDefenderExclusion(const std::string& path);
bool SetRegistryDwordValue(HKEY hKeyRoot, const std::string& subKey, const std::string& valueName, DWORD value);

std::string base64_encode(const std::string& input);
std::string base64_decode(const std::string& input);
uint32_t hash_string(const char* s);
#endif // BLIB_H