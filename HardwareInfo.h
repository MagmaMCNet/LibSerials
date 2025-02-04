#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <fstream>
#include <list>
#include <map>
#include <array>
#include <intrin.h>
#include <dxgi.h>
#include <tchar.h>
#include <iphlpapi.h>
#include <memory>
#include <comdef.h>
#include <Wbemidl.h>
#include "SMBIOS.h"
#include "UtilityFunctions.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wbemuuid.lib")


class HardwareInfo {
public:
	static std::string GetCPU();
	static std::string GetGPU();
	static std::vector<std::string> GetMacAddresses();
	static size_t GetTotalMemory();
	static std::string GetCPUSerialNumber();
	static std::string GetBiosSerialNumber();
	static std::string GetSMBiosUUID();
	static std::vector<std::string> GetDriveSerialNumbers();
	static std::vector<std::string> GetDrivesLetters();
	static std::string GetVolumeSerialNumber(const std::string& volume);
};