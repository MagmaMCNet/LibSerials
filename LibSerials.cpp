#include <Windows.h>
#include <iostream>
#include <iomanip>
#include "HardwareInfo.h"
#include "StringUtils.h"
#include "LibSerials.h"

const static std::string Title = "Serial Info";

std::string HashComponent(const std::string input) {
    std::hash<std::string> hasher;
    size_t hash = hasher(input);
    std::stringstream ss;
    ss << std::uppercase << std::hex << (hash & 0xFFFFF);
    return ss.str();
}
bool AttachConsole() {
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        if (AllocConsole()) {
            FILE* fp;
            if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0)
                return false;
            if (freopen_s(&fp, "CONIN$", "r", stdin) != 0)
                return false;
        }
    }
    else {
        FILE* fp;
        if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0)
            return false;
        if (freopen_s(&fp, "CONIN$", "r", stdin) != 0)
            return false;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hConsole, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsole, dwMode);
    }

    system("cls");
    return true;
}

EXPORT void Initilize() {
    HardwareInfo::InitilizeSMBIOS();
}
EXPORT void Free() {
    HardwareInfo::FreeSMBIOS();
}
EXPORTCPP const std::vector<std::string> HDD_Serials() {
    auto HDDSerials = HardwareInfo::GetDriveSerialNumbers();
    HDDSerials.erase(
        std::remove_if(HDDSerials.begin(), HDDSerials.end(), [](std::string& serial) {
            serial = String::Trim(serial);
            return serial.empty();
            }),
        HDDSerials.end()
    );

    if (HDDSerials.empty())
        HDDSerials.push_back("000-000-000");

    return HDDSerials;
}
EXPORT const std::string BIOS_Vendor() {
    return String::ToASCII(HardwareInfo::BIOSInformation.size() > 0 ? HardwareInfo::BIOSInformation.at(0) : "N/A");
}
EXPORT const std::string BIOS_Version() {
    return String::ToASCII(HardwareInfo::BIOSInformation.size() > 1 ? HardwareInfo::BIOSInformation.at(1) : "1000");
}
EXPORT const std::string BIOS_Date() {
    return String::ToASCII(HardwareInfo::BIOSInformation.size() > 2 ? HardwareInfo::BIOSInformation.at(2) : "01/01/2025");
}

EXPORT const std::string Chassis_Manufacturer() {
    return String::ToASCII(HardwareInfo::ChassisInformation.size() > 0 ? HardwareInfo::ChassisInformation.at(0) : "N/A");
}
EXPORT const std::string Chassis_Version() {
    return String::ToASCII(HardwareInfo::ChassisInformation.size() > 2 ? HardwareInfo::ChassisInformation.at(2) : "N/A");
}
EXPORT const std::string Chassis_Serial() {
    return String::ToASCII(HardwareInfo::ChassisInformation.size() > 3 ? HardwareInfo::ChassisInformation.at(3) : "N/A");
}

EXPORT const std::string System_Manufacturer() {
    return String::ToASCII(HardwareInfo::SystemInformation.size() > 0 ? HardwareInfo::SystemInformation.at(0) : "N/A");
}
EXPORT const std::string System_Product() {
    return String::ToASCII(HardwareInfo::SystemInformation.size() > 1 ? HardwareInfo::SystemInformation.at(1) : "N/A");
}
EXPORT const std::string System_Version() {
    return String::ToASCII(HardwareInfo::SystemInformation.size() > 2 ? HardwareInfo::SystemInformation.at(2) : "N/A");
}
EXPORT const std::string System_Serial() {
    return String::ToASCII(HardwareInfo::SystemInformation.size() > 3 ? HardwareInfo::SystemInformation.at(3) : "N/A");
}
EXPORT const std::string System_SKU() {
    return String::ToASCII(HardwareInfo::SystemInformation.size() > 4 ? HardwareInfo::SystemInformation.at(4) : "N/A");
}
EXPORT const std::string System_Family() {
    return String::ToASCII(HardwareInfo::SystemInformation.size() > 5 ? HardwareInfo::SystemInformation.at(5) : "N/A");
}
EXPORT const std::string System_UUID() {
    std::string BIOSSerial = String::ToASCII(HardwareInfo::GetSMBiosUUID());
    return BIOSSerial.empty() ? "0000-000-000-000-0000" : BIOSSerial;
}

EXPORT const std::string Baseboard_Manufacturer() {
    return String::ToASCII(HardwareInfo::BaseBoardInformation.size() > 0 ? HardwareInfo::BaseBoardInformation.at(0) : "N/A");
}
EXPORT const std::string Baseboard_Product() {
    return String::ToASCII(HardwareInfo::BaseBoardInformation.size() > 1 ? HardwareInfo::BaseBoardInformation.at(1) : "N/A");
}
EXPORT const std::string Baseboard_Serial() {
    std::string Serial = String::ToASCII(HardwareInfo::GetBaseboardSerialNumber());
    return Serial.empty() ? "000000000000000" : Serial;
}
EXPORT const std::string CPU_Product() {
    return String::Trim(HardwareInfo::GetCPU());
}
EXPORT const std::string CPU_Serial() {
    return String::Trim(HardwareInfo::GetCPUSerialNumber());
}
EXPORTCPP const std::vector<std::string> MACAddress_List() {
    return HardwareInfo::GetMacAddresses();
}
EXPORT const std::string MACAddress_Main() {
    IP_ADAPTER_INFO adapterInfo[16];
    DWORD bufLen = sizeof(adapterInfo);

    DWORD status = GetAdaptersInfo(adapterInfo, &bufLen);
    if (status != ERROR_SUCCESS)
        return "00:00:00:00:00:00";

    for (PIP_ADAPTER_INFO pAdapter = adapterInfo; pAdapter != nullptr; pAdapter = pAdapter->Next) {
        if (strstr(pAdapter->Description, "VMware") != nullptr)
            continue;

        if (pAdapter->AddressLength != 0) {
            std::ostringstream macAddress;
            for (UINT i = 0; i < pAdapter->AddressLength; i++) {
                macAddress << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)pAdapter->Address[i];
                if (i != pAdapter->AddressLength - 1)
                    macAddress << ":";
            }
            return macAddress.str();
        }
    }
    return "00:00:00:00:00:00";
}
EXPORT const int Component_GetPercentage(const Component item) {
    switch (item)
    {
    case Component::Baseboard:
        return Baseboard_Change;
    case Component::BIOS:
        return BIOS_Change;
    case Component::SMBIOS:
        return SMBIOS_Change;
    case Component::CPU:
        return CPU_Change;
    case Component::MAC:
        return MAC_Change;
    default:
        break;
    }
}
EXPORT void Component_SetPercentage(const Component item, const int value) {
    switch (item)
    {
    case Component::Baseboard:
        Baseboard_Change = value;
        break;
    case Component::BIOS:
        BIOS_Change = value;
        break;
    case Component::SMBIOS:
        SMBIOS_Change = value;
        break;
    case Component::CPU:
        CPU_Change = value;
        break;
    case Component::MAC:
        MAC_Change = value;
        break;
    default:
        break;
    }
}
EXPORT std::string System_HWID() {
    std::ostringstream hwid;
    hwid << HashComponent(Baseboard_Serial()) << ":"
        << HashComponent(System_UUID()) << ":"
        << HashComponent(CPU_Serial()) << ":"
        << HashComponent(MACAddress_Main());
    return hwid.str();
}
EXPORT bool System_ValidateHWID(const char* SavedID) {
    std::string currentHWID = System_HWID();

    auto split = [](const std::string& str, char delimiter) {
        std::vector<std::string> components;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delimiter))
            components.push_back(item);
        return components;
        };

    auto currentComponents = split(currentHWID, ':');

    auto expectedComponents = split(SavedID, ':');

    if (currentComponents.size() != expectedComponents.size())
        return false;

    int totalDifference = 0;

    if (currentComponents[0] != expectedComponents[0]) totalDifference += BIOS_Change;
    if (currentComponents[1] != expectedComponents[1]) totalDifference += SMBIOS_Change;
    if (currentComponents[2] != expectedComponents[2]) totalDifference += CPU_Change;
    if (currentComponents[3] != expectedComponents[3]) totalDifference += MAC_Change;

    return totalDifference <= 40;
}

#pragma region Debug
EXPORT void Debug() {
    AttachConsole();
    Initilize();

    std::vector<std::string> HDDSerials = HDD_Serials();
    std::cout << String::Colorize(" [!] SSD/HDD\n", "ff2a00", "ff8c00");
    for (int i = 0; i < HDDSerials.size(); i++)
        std::cout << String::Colorize("[" + std::to_string(i) + "] ", "ff2a00", "ff8c00") + HDDSerials.at(i) + "\n";

    std::cout << String::Colorize("\n [!] Motherboard\n", "ff8c00", "62ff00");
    std::cout << String::Colorize("[Manufacturer] ", "ff8c00", "62ff00") + Baseboard_Manufacturer() + "\n";
    std::cout << String::Colorize("[Product]      ", "ff8c00", "62ff00") + Baseboard_Product() + "\n";
    std::cout << String::Colorize("[Serial]       ", "ff8c00", "62ff00") + Baseboard_Serial() + "\n\n";

    std::cout << String::Colorize(" [!] BIOS\n", "09ff00", "00eaff");
    std::cout << String::Colorize("[Vendor]  ", "09ff00", "00eaff") + BIOS_Vendor() + "\n";
    std::cout << String::Colorize("[Version] ", "09ff00", "00eaff") + BIOS_Version() + "\n\n";

    std::cout << String::Colorize(" [!] System\n", "00eaff", "0022ff");
    std::cout << String::Colorize("[Manufacturer] ", "00eaff", "0022ff") + System_Manufacturer() + "\n";
    std::cout << String::Colorize("[UUID]         ", "00eaff", "0022ff") + System_UUID() + "\n";
    std::cout << String::Colorize("[Family]       ", "00eaff", "0022ff") + System_Family() + "\n\n";

    std::cout << String::Colorize(" [!] Chassis\n", "6E11FF", "dd00ff");
    std::cout << String::Colorize("[Manufacturer] ", "6E11FF", "dd00ff") + Chassis_Manufacturer() + "\n";
    std::cout << String::Colorize("[Version]      ", "6E11FF", "dd00ff") + Chassis_Version() + "\n";
    std::cout << String::Colorize("[Serial]       ", "6E11FF", "dd00ff") + Chassis_Serial() + "\n\n";

    std::cout << String::Colorize(" [!] CPU\n", "dd00ff", "ff00ea");
    std::cout << String::Colorize("[Name]   ", "dd00ff", "ff00ea") + CPU_Product() + "\n";
    std::cout << String::Colorize("[Serial] ", "dd00ff", "e600ff") + CPU_Serial() + "\n\n";

    std::vector<std::string> MacAddresses = MACAddress_List();
    std::cout << String::Colorize(" [!] MAC Address\n", "e600ff", "ff0000");
    for (int i = 0; i < MacAddresses.size(); i++)
        std::cout << String::Colorize("[" + std::to_string(i) + "] ", "f520c3", "fa202f") + MacAddresses.at(i) + "\n";

    std::string hwid = System_HWID();
    std::cout << String::Colorize("\n[*] HWID: " + hwid, "ff2a00", "ff8c00") << std::endl;
    std::cout << String::Colorize("[*] github.com/MagmaMCNet/LibSerials", "2e2e2e") << std::endl;
    std::cin.ignore();
    FreeConsole();
    _exit(0);
}
void main() {
    Debug();
}
#pragma endregion
