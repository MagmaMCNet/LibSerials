#include "EXText.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include "SerialReader.h"
#include "BLib.h"
#include "AssignedSerial.h"
#include <functional>
#include <map>
#include "HardwareInfo.h"
#include <iomanip>

static void WriteFile(const std::string& filename, const std::string& data) {
    std::ofstream out(filename, std::ios::binary);
    if (out.is_open()) {
        out.write(data.data(), data.size());
        out.close();
    }
}

static void allocateConsole() {
    if (AllocConsole()) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONIN$", "r", stdin);
    }
    EnableANSIColors(true);
}

static std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        return "";
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}


extern "C" __declspec(dllexport) void GenerateSerials() {
    std::cout << "-- CPU --" << std::endl;
    std::cout << HardwareInfo::GetCPU() << std::endl;
    std::cout << "-- GPU --" << std::endl;
    std::cout << HardwareInfo::GetGPU() << std::endl;
    std::cout << "-- Mac Addresses --" << std::endl;
    std::vector<std::string> macaddresses = HardwareInfo::GetMacAddresses();
    for (size_t i = 0; i < macaddresses.size(); i++)
        std::cout << "[" + std::to_string(i) + "] " + macaddresses.at(i) << std::endl;
    std::cout << "-- Memory --" << std::endl;
    double totalMemory = HardwareInfo::GetTotalMemory() / 1024.0;
    std::cout << "Total Memory Count: "
        << std::fixed << std::setprecision(1)
        << totalMemory << "GB" << std::endl;
    std::cout << "-- Drives --" << std::endl;
    std::vector<std::string> DriveSerials = HardwareInfo::GetDriveSerialNumbers();
    for (size_t i = 0; i < DriveSerials.size(); i++)
        std::cout << "[" + std::to_string(i) + "] " + DriveSerials.at(i) << std::endl;

}

extern "C" __declspec(dllexport) std::string ReadSavedSerials() {
    try {
        std::string FileData = ReadFile(SERIALFILE);
        return EXText::decrypt(FileData, PASSWORD);
    }
    catch (...) {}
    return "[NULL]";
}

extern "C" __declspec(dllexport) void ViewSavedSerials() {
    SerialReader Reader(EXText::decrypt(ReadFile(SERIALFILE), PASSWORD));
    for (const auto& [name, value] : Reader.GetRows())
        std::cout << Reader.FormatRow("[" + name + "]", value) << std::endl;
}

#ifdef TRUE || DEBUG
extern "C" __declspec(dllexport) void ViewExampleSerials() {
    std::string data = "[Serial] 17267\n[Motherboard] B5000-Ligma\n[Manufacture] Ligma Inc\n[OS] Windows 10";
    SerialReader reader(data);

    std::string value;

    for (const auto& [name, value] : reader.GetRows())
        std::cout << reader.FormatRow("[" + name + "]", value) << std::endl;

    std::cout << "\n\033[1;35m[Exported Data] \033[1;37m\n\033[1;34m-------------------------\033[0m" << std::endl;
    std::cout << reader.Export() << std::endl;
    WriteFile(SERIALFILE+"_Debug", EXText::encrypt(reader.Export(), PASSWORD));
    return;
}
extern "C" __declspec(dllexport) void Lib_Encrypt() {
    std::cout << "\033[1;36m[DEBUG MODE]\033[0m" << std::endl;
    try {
        std::cout << "\033[1;33mReading Data...\033[0m" << std::endl;
        std::string fileContent = ReadFile(SERIALFILE);

        if (EXText::isEncrypted(fileContent))
            std::cout << "\033[1;31mError:\033[0m The file is already encrypted.\n" << std::endl;
        else {
            std::cout << "\033[1;32mEncrypting...\033[0m" << std::endl;
            std::string encryptedData = EXText::encrypt(fileContent, PASSWORD);
            WriteFile(SERIALFILE, encryptedData);
            std::cout << "\033[1;32mEncryption Complete.\033[0m Encrypted file saved as \033[1;36m" << SERIALFILE << "\033[0m." << std::endl;
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "\033[1;31mError:\033[0m " << ex.what() << std::endl;
    }
}

extern "C" __declspec(dllexport) void Lib_Decrypt() {
    std::cout << "\033[1;36m[DEBUG MODE]\033[0m" << std::endl;
    try {
        std::cout << "\033[1;33mReading Data...\033[0m" << std::endl;
        std::string fileContent = ReadFile(SERIALFILE);

        if (!EXText::isEncrypted(fileContent))
            std::cout << "\033[1;31mError:\033[0m The file is not encrypted.\n" << std::endl;
        else {
            std::cout << "\033[1;32mDecrypting...\033[0m" << std::endl;
            std::string decryptedData = EXText::decrypt(fileContent, PASSWORD);
            WriteFile(SERIALFILE, decryptedData);
            std::cout << "\033[1;32mDecryption Complete.\033[0m Decrypted file saved as \033[1;36m" << SERIALFILE << "\033[0m." << std::endl;
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "\033[1;31mError:\033[0m " << ex.what() << std::endl;
    }
}
extern "C" __declspec(dllexport) void DebugUI() {
    allocateConsole();
    SetConsoleTitleA("Debug UI");

    // Map of function names to callable functions
    std::map<int, std::function<void()>> functionMap;

    // Bind functions to the map
    functionMap[1] = GenerateSerials;
    functionMap[2] = ViewSavedSerials;
    functionMap[3] = ViewExampleSerials;
    functionMap[4] = Lib_Encrypt;
    functionMap[5] = Lib_Decrypt;

    bool running = true;

    while (running) {
        std::cout << "\n\033[1;36m--- Debug UI ---\033[0m\n";
        std::cout << "1. Generate Serial\n";
        std::cout << "2. View Saved Serials\n";
        std::cout << "3. View Example Serials\n";
        std::cout << "4. Encrypt File\n";
        std::cout << "5. Decrypt File\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose an option: ";

        int choice;
        std::cin >> choice;

        if (choice == 0) {
            std::cout << "Exiting Debug UI..." << std::endl;
            running = false;
        }
        else if (functionMap.find(choice) != functionMap.end()) {
            system("cls");
            functionMap[choice]();
            std::cout << "\n\033[1;33mPress Enter to exit...\033[0m";
            system("pause >nul");
            system("cls");
        }
        else {
            std::cout << "\033[1;31mInvalid choice. Please try again.\033[0m" << std::endl;
        }
    }
}
void main() {
    DebugUI();
}
#else
extern "C" __declspec(dllexport) void DebugUI();
extern "C" __declspec(dllexport) void Lib_Encrypt();
extern "C" __declspec(dllexport) void Lib_Decrypt();
#endif