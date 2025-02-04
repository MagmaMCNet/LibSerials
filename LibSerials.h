#pragma once

#include <vector>
#include <string>

// Static percentage values for component changes
static int Baseboard_Change = 20;
static int BIOS_Change = 20;
static int SMBIOS_Change = 30;
static int CPU_Change = 35;
static int MAC_Change = 10;

// Enum for component types
enum class Component {
    Baseboard,
    BIOS,
    SMBIOS,
    CPU,
    MAC
};

// Exported functions
#define EXPORT extern "C" __declspec(dllexport)
#define EXPORTCPP extern "C++" __declspec(dllexport)

// Library initialization and cleanup
EXPORT void Initilize();
EXPORT void Free();

// Hardware information functions
EXPORTCPP const std::vector<std::string> HDD_Serials();
EXPORT const std::string Baseboard_Manufacturer();
EXPORT const std::string Baseboard_Product();
EXPORT const std::string Baseboard_Serial();
EXPORT const std::string BIOS_Serial();
EXPORT const std::string SMBIOS_UUID();
EXPORT const std::string CPU_Product();
EXPORT const std::string CPU_Serial();
EXPORTCPP const std::vector<std::string> MACAddress_List();
EXPORT const std::string MACAddress_Main();

// Component percentage management
EXPORT const int Component_GetPercentage(const Component item);
EXPORT void Component_SetPercentage(const Component item, const int value);

// System information functions
EXPORT std::string System_HWID();
EXPORT bool System_ValidateHWID(const char* SavedID);

