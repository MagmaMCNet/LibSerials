#include "EXText.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include "SerialReader.h"
#include "BLib.h"

const std::string PASSWORD = "AssignedSerial";

void WriteFile(const std::string& filename, const std::string& data) {
    std::ofstream out(filename, std::ios::binary);
    if (out.is_open()) {
        out.write(data.data(), data.size());
        out.close();
    }
}

void allocateConsole() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
}

std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + filename);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

int main() {
    try {
        std::cout << "Reading Data... main" << std::endl;
        std::string encryptedData = ReadFile("HWID");

        std::string decryptedData = EXText::decrypt(encryptedData, PASSWORD);
        std::cout << "Decrypted Data: " << decryptedData << std::endl;

        std::cout << "Exit..." << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}

extern "C" __declspec(dllexport) void GenerateSerials() {

}

extern "C" __declspec(dllexport) void ReadExampleSerial() {
    allocateConsole();
    EnableANSIColors(true);

    std::string data = "[Serial] 17267\n[Motherboard] B5000-Ligma\n[Manufacture] Ligma Inc";
    SerialReader reader(data);

    reader.WriteRow("OS", "Windows 10");

    std::string value;
    size_t maxLength = 0;

    if (reader.ReadRow("Serial", value))
        maxLength = max(maxLength, size_t(strlen("[Serial]")));

    if (reader.ReadRow("Motherboard", value))
        maxLength = max(maxLength, size_t(strlen("[Motherboard]")));

    if (reader.ReadRow("Manufacture", value))
        maxLength = max(maxLength, size_t(strlen("[Manufacture]")));

    if (reader.ReadRow("OS", value))
        maxLength = max(maxLength, size_t(strlen("[OS]")));

    // Stylish output formatting
    auto formatWithPadding = [&maxLength](const std::string& title, const std::string& value) {
        size_t titleLength = title.length();
        size_t padding = maxLength - titleLength;
        std::string paddedTitle = title + std::string(padding, ' ');  // Add spaces to align
        return "\033[1;36m" + paddedTitle + "\033[1;37m \t" + value;
        };

    if (reader.ReadRow("Serial", value))
        std::cout << formatWithPadding("[Serial]", value) << std::endl;

    if (reader.ReadRow("Motherboard", value))
        std::cout << formatWithPadding("[Motherboard]", value) << std::endl;

    if (reader.ReadRow("Manufacture", value))
        std::cout << formatWithPadding("[Manufacture]", value) << std::endl;

    if (reader.ReadRow("OS", value))
        std::cout << formatWithPadding("[OS]", value) << std::endl;

    std::cout << "\n\033[1;35m[Exported Data] \033[1;37m\n\033[1;34m-------------------------\033[0m" << std::endl;
    std::cout << reader.Export() << std::endl;

    std::cout << "\n\033[1;33mPress Enter to exit...\033[0m";
    std::cin.ignore();

    return;
}

extern "C" __declspec(dllexport) std::string ReadSavedSerial() {
    try {
        std::string FileData = ReadFile("HWID");
        return EXText::decrypt(FileData, PASSWORD);
    }
    catch (...) {}
    return "[NULL]";
}
extern "C" __declspec(dllexport) std::string Lib_ViewSavedSerial() {
    allocateConsole();
    try {
        std::string FileData = ReadFile("HWID");
        std::cout << EXText::decrypt(FileData, PASSWORD) << std::endl;
        std::cin.ignore();
    }
    catch (...) {}
    return "[NULL]";
}
#ifdef _DEBUG
extern "C" __declspec(dllexport) void Lib_Encrypt() {
    allocateConsole();
    std::cout << "[DEBUG MODE]" << std::endl;
    try {
        std::cout << "Reading Data..." << std::endl;
        std::string fileContent = ReadFile("HWID");
        std::string encryptedData = EXText::encrypt(fileContent, PASSWORD);
        WriteFile("HWID_encrypted", encryptedData);

        std::cout << "Encryption Complete. Encrypted file saved as HWID_encrypted." << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    system("pause");
}

extern "C" __declspec(dllexport) void Lib_Decrypt() {
    allocateConsole();
    try {
        std::cout << "[DEBUG MODE]" << std::endl;
        std::cout << "Reading Data..." << std::endl;
        std::string encryptedData = ReadFile("HWID_encrypted");
        std::string decryptedData = EXText::decrypt(encryptedData, PASSWORD);
        WriteFile("HWID_decrypted", decryptedData);

        std::cout << "Decryption Complete. Decrypted file saved as HWID_decrypted." << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    system("pause");
}
#else
extern "C" __declspec(dllexport) void Lib_Encrypt();
extern "C" __declspec(dllexport) void Lib_Decrypt();
#endif