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


namespace HardwareInfo {
	/// <summary>
	/// Part 2-1. Base Board (Type 2)
	/// </summary>
	extern std::vector<std::string> BaseBoardInformation;
	extern std::vector<std::string> PhysicalMemoryInformation;
	extern std::vector<std::string> PhysicalMemorySerials;
	/// <summary>
	/// Part 0. System (Type 0)
	/// </summary>
	extern std::vector<std::string> BIOSInformation;
	/// <summary>
	/// Part 1. System (Type 1)
	/// </summary>
	extern std::vector<std::string> SystemInformation;
	/// <summary>
	/// Part 3-1. Chassis (Type 3)
	/// </summary>
	extern std::vector<std::string> ChassisInformation;
	/// <summary>
	/// Part 4. Processor Information (Type 4)
	/// </summary>
	extern std::vector<std::string> ProcessorInformation;

	std::string GetCPU();
	std::string GetGPU();
	std::vector<std::string> GetMacAddresses();
	size_t GetTotalMemory();
	std::string GetCPUSerialNumber();
	std::string GetBaseboardSerialNumber();
	std::string GetSMBiosUUID();
	std::vector<std::string> GetDriveSerialNumbers();
	std::vector<std::string> GetDrivesLetters();
	std::string GetVolumeSerialNumber(const std::string& volume);

	void InitializeSMBIOS();
	void FreeSMBIOS();
};