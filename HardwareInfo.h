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

class HardwareInfo {
public:
	static std::string GetCPU();
	static std::string GetGPU();
	static std::vector<std::string> GetMacAddresses();
	static size_t GetTotalMemory();
	static std::vector<std::string> GetDriveSerialNumbers();
};