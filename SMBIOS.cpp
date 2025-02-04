#include <Windows.h>
#include <sysinfoapi.h>
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
#include "SMBIOS.h"

/// Modified Version To Fix Memory Leak
/// https://github.com/IntelSDM/AdvancedHardwareCollection/blob/master/UserModeHardwareCollection/SMBIOS.cpp
std::vector<std::string> BaseBoardInformation;
std::string BaseBoardSerial;
std::vector<std::string> PhysicalMemoryInformation;
std::vector<std::string> PhysicalMemorySerials;
RawSMBIOSData* RawData;
RawSMBIOSData* GetRawData()
{
	DWORD error = ERROR_SUCCESS;
	DWORD smBiosDataSize = 0;
	RawSMBIOSData* smBiosData = NULL;
	DWORD bytesWritten = 0;

	// Make sure that there is enough space in the heap for this table
	smBiosDataSize = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
	smBiosData = (RawSMBIOSData*)HeapAlloc(GetProcessHeap(), 0, smBiosDataSize);
	if (!smBiosData) {
		error = ERROR_OUTOFMEMORY;
		exit(1);
	}

	// Make sure that the data used is valid (by checking the amount of data received)
	bytesWritten = GetSystemFirmwareTable('RSMB', 0, smBiosData, smBiosDataSize);
	if (bytesWritten != smBiosDataSize) {
		error = ERROR_INVALID_DATA;
		exit(1);
	}

	return smBiosData;
}


SMBIOSStruct* GetNextStruct(SMBIOSStruct* curStruct)
{
	char* strings_begin = (char*)curStruct + curStruct->Length;
	char* next_strings = strings_begin + 1;
	while (*strings_begin != NULL || *next_strings != NULL)
	{
		++strings_begin;
		++next_strings;
	}
	return (SMBIOSStruct*)(next_strings + 1);
}

std::vector<SMBIOSStruct*> GetStructureTable(RawSMBIOSData* rawdata)
{
	std::vector<SMBIOSStruct*> structure_table;
	SMBIOSStruct* curStruct = (SMBIOSStruct*)rawdata->SMBIOSTableData;
	while ((char*)curStruct < (char*)rawdata + rawdata->Length)
	{
		structure_table.push_back(curStruct);
		curStruct = GetNextStruct(curStruct);
	}
	return structure_table;
}
std::vector<std::string> ConvertSMBIOSString(SMBIOSStruct* curStruct)
{
	std::vector<std::string> strings;
	std::string res = "";
	strings.push_back(res);
	char* cur_char = (char*)curStruct + curStruct->Length;
	SMBIOSStruct* next_struct = GetNextStruct(curStruct);

	while (cur_char < (char*)next_struct)
	{
		res.push_back(*cur_char);
		if (*cur_char == NULL)
		{
			strings.push_back(res);
			res = "";
		}
		++cur_char;
	}
	return strings;
}
void GetPhysicalMemoryInformation(SMBIOSPhysicalMemory* curStruct, RawSMBIOSData* rawdata)
{
    auto strings = ConvertSMBIOSString(curStruct);

    if (static_cast<int>(curStruct->Size) == 0)
        return;

    if (rawdata->SMBIOSMajorVersion < 2 || (rawdata->SMBIOSMajorVersion == 2 && rawdata->SMBIOSMinorVersion < 1))
    {
        return;
    }

    PhysicalMemoryInformation.emplace_back(std::to_string(static_cast<int>(curStruct->Size)) + "mb");
    PhysicalMemoryInformation.emplace_back(strings.at(curStruct->BankLocator));

    if (rawdata->SMBIOSMajorVersion == 2 && rawdata->SMBIOSMinorVersion < 3) {
        std::cout << std::endl;
        return;
    }

    PhysicalMemoryInformation.emplace_back(strings.at(curStruct->SerialNumber));
    PhysicalMemoryInformation.emplace_back(strings.at(curStruct->AssetTag));
    PhysicalMemorySerials.emplace_back(strings.at(curStruct->SerialNumber));
    PhysicalMemoryInformation.emplace_back("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"); // separator for visibility

    if (rawdata->SMBIOSMajorVersion == 2 && rawdata->SMBIOSMinorVersion < 6) {
        return;
    }
}

void GetBaseBoardInformation(SMBIOSBaseBoard* curStruct, RawSMBIOSData* rawdata)
{
    auto strings = ConvertSMBIOSString(curStruct);
    
    BaseBoardSerial = strings.at(curStruct->SerialNumber) + strings.at(curStruct->Product);  // better concatenation to distinguish products.

    BaseBoardInformation.emplace_back(strings.at(curStruct->Manufacturer));
    BaseBoardInformation.emplace_back(strings.at(curStruct->Product));
    BaseBoardInformation.emplace_back(strings.at(curStruct->SerialNumber));
}
void ConvertData(RawSMBIOSData* rawdata, int id)
{
	std::vector<SMBIOSStruct*> structureTable = GetStructureTable(rawdata);
	switch (structureTable[id]->Type)
	{
	case 2:
		GetBaseBoardInformation((SMBIOSBaseBoard*)structureTable[id], rawdata);
		break;
	case 17:
		GetPhysicalMemoryInformation((SMBIOSPhysicalMemory*)structureTable[id], rawdata);
		break;
	}
}
void GetBIOSInfo()
{
	RawData = GetRawData();
	std::vector<SMBIOSStruct*> structureTable = GetStructureTable(RawData);
	for (int i = 0; i < structureTable.size(); ++i)
		ConvertData(RawData, i);
}
void FreeMemory() {
	PhysicalMemoryInformation.clear();
	PhysicalMemorySerials.clear();
	BaseBoardInformation.clear();
	BaseBoardSerial.clear();
	PhysicalMemoryInformation.shrink_to_fit();
	PhysicalMemorySerials.shrink_to_fit();
	BaseBoardInformation.shrink_to_fit();
	free(RawData);
}