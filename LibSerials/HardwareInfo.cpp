#include "HardwareInfo.h"
#include "StringUtils.h"
namespace HardwareInfo {

    std::vector<std::string> BaseBoardInformation;
    std::vector<std::string> PhysicalMemoryInformation;
    std::vector<std::string> PhysicalMemorySerials;
    std::vector<std::string> BIOSInformation;
    std::vector<std::string> SystemInformation;
    std::vector<std::string> ChassisInformation;
    std::vector<std::string> ProcessorInformation;

    RawSMBIOSData* RawData;

    RawSMBIOSData* GetRawData() {
        DWORD smBiosDataSize = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
        RawSMBIOSData* smBiosData = (RawSMBIOSData*)HeapAlloc(GetProcessHeap(), 0, smBiosDataSize);
        if (!smBiosData) exit(1);

        DWORD bytesWritten = GetSystemFirmwareTable('RSMB', 0, smBiosData, smBiosDataSize);
        if (bytesWritten != smBiosDataSize) exit(1);

        return smBiosData;
    }

    SMBIOSStruct* GetNextStruct(SMBIOSStruct* curStruct) {
        char* strings_begin = (char*)curStruct + curStruct->Length;
        char* next_strings = strings_begin + 1;
        while (*strings_begin != NULL || *next_strings != NULL) {
            ++strings_begin;
            ++next_strings;
        }
        return (SMBIOSStruct*)(next_strings + 1);
    }

    std::vector<SMBIOSStruct*> GetStructureTable(RawSMBIOSData* rawdata) {
        std::vector<SMBIOSStruct*> structure_table;
        SMBIOSStruct* curStruct = (SMBIOSStruct*)rawdata->SMBIOSTableData;
        while ((char*)curStruct < (char*)rawdata + rawdata->Length) {
            structure_table.push_back(curStruct);
            curStruct = GetNextStruct(curStruct);
        }
        return structure_table;
    }

    std::vector<std::string> ConvertSMBIOSString(SMBIOSStruct* curStruct) {
        std::vector<std::string> strings;
        std::string res = "";
        strings.push_back(res);
        char* cur_char = (char*)curStruct + curStruct->Length;
        SMBIOSStruct* next_struct = GetNextStruct(curStruct);

        while (cur_char < (char*)next_struct) {
            res.push_back(*cur_char);
            if (*cur_char == NULL) {
                strings.push_back(String::ToASCII(res));
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
        BaseBoardInformation.emplace_back(strings.at(curStruct->Manufacturer));
        BaseBoardInformation.emplace_back(strings.at(curStruct->Product));
        BaseBoardInformation.emplace_back(strings.at(curStruct->SerialNumber));
    }
    void GetBIOSInformation(SMBIOSBIOSInfo* curStruct) {
        auto strings = ConvertSMBIOSString(curStruct);
        BIOSInformation.emplace_back(strings.at(curStruct->Vendor));
        BIOSInformation.emplace_back(strings.at(curStruct->Version));
        BIOSInformation.emplace_back(strings.at(curStruct->ReleaseDate));
    }
    void GetSystemInformation(SMBIOSSystemInfo* curStruct) {
        auto strings = ConvertSMBIOSString(curStruct);
        SystemInformation.emplace_back(strings.at(curStruct->Manufacturer));
        SystemInformation.emplace_back(strings.at(curStruct->ProductName));
        SystemInformation.emplace_back(strings.at(curStruct->Version));
        SystemInformation.emplace_back(strings.at(curStruct->SerialNumber));
        SystemInformation.emplace_back(strings.at(curStruct->SKUNum));
        SystemInformation.emplace_back(strings.at(curStruct->Family));
    }
    void GetChassisInformation(SMBIOSChassis* curStruct) {
        auto strings = ConvertSMBIOSString(curStruct);
        ChassisInformation.emplace_back(strings.at(curStruct->Manufacturer));
        ChassisInformation.emplace_back(std::to_string(curStruct->Type));
        ChassisInformation.emplace_back(strings.at(curStruct->Version));
        ChassisInformation.emplace_back(strings.at(curStruct->SerialNumber));
    }
    void GetProcessorInformation(SMBIOSProcessorInfo* curStruct) {
        auto strings = ConvertSMBIOSString(curStruct);
        ProcessorInformation.emplace_back(strings.at(curStruct->SerialNumber));
        ProcessorInformation.emplace_back(strings.at(curStruct->AssetTag));
        ProcessorInformation.emplace_back(strings.at(curStruct->PartNumber));
    }

    void ConvertData(RawSMBIOSData* rawdata, int id) {
        std::vector<SMBIOSStruct*> structureTable = GetStructureTable(rawdata);
        switch (structureTable[id]->Type) {
        case 0:
            GetBIOSInformation((SMBIOSBIOSInfo*)structureTable[id]);
            break;
        case 1:
            GetSystemInformation((SMBIOSSystemInfo*)structureTable[id]);
            break;
        case 2:
            GetBaseBoardInformation((SMBIOSBaseBoard*)structureTable[id], rawdata);
            break;
        case 3:
            GetChassisInformation((SMBIOSChassis*)structureTable[id]);
            break;
        case 17:
            GetPhysicalMemoryInformation((SMBIOSPhysicalMemory*)structureTable[id], rawdata);
            break;
            // Additional cases for types 11, 12, 22, and 39 can be added here
        }
    }

    void InitializeSMBIOS() {
        RawData = GetRawData();
        std::vector<SMBIOSStruct*> structureTable = GetStructureTable(RawData);
        for (int i = 0; i < structureTable.size(); ++i) {
            ConvertData(RawData, i);
        }
    }

    void FreeSMBIOS() {
        PhysicalMemoryInformation.clear();
        PhysicalMemorySerials.clear();
        BaseBoardInformation.clear();
        BIOSInformation.clear();
        SystemInformation.clear();
        ChassisInformation.clear();
        ProcessorInformation.clear();
        PhysicalMemoryInformation.shrink_to_fit();
        PhysicalMemorySerials.shrink_to_fit();
        BaseBoardInformation.shrink_to_fit();
        BIOSInformation.shrink_to_fit();
        SystemInformation.shrink_to_fit();
        ChassisInformation.shrink_to_fit();
        ProcessorInformation.shrink_to_fit();
        if (RawData) {
            memset(RawData, 0, sizeof(RawSMBIOSData));
            HeapFree(GetProcessHeap(), 0, RawData);
            RawData = NULL;
        }
    }

    std::string GetCPU() {
        std::array<int, 4> integerbuffer;
        constexpr size_t sizeofintegerbuffer = sizeof(int) * integerbuffer.size();

        std::array<char, 64> charbuffer = {};
        constexpr std::array<int, 3> functionids = {
            0x8000'0002, // Manufacturer
            0x8000'0003, // Model
            0x8000'0004  // Clockspeed
        };

        std::string cpuname;

        for (int id : functionids) {
            __cpuid(integerbuffer.data(), id);
            std::memcpy(charbuffer.data(), integerbuffer.data(), sizeofintegerbuffer);
            cpuname += std::string(charbuffer.data());
        }
        return cpuname;
    }
    std::string GetCPUSerialNumber() {
        std::array<int, 4> buffer;
        const int serialFeatureId = 0x8000'0008;
        __cpuid(buffer.data(), serialFeatureId);
        if (buffer[0] != 0 || buffer[1] != 0 || buffer[2] != 0 || buffer[3] != 0) {
            std::stringstream serialStream;
            serialStream << std::hex << std::uppercase
                << buffer[0] << buffer[1]
                << buffer[2] << buffer[3];
            return serialStream.str();
        }
        return "CPU Serial Not Available";
    }
    std::string GetBaseboardSerialNumber() {
        if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) return "Failed to initialize COM";

        if (FAILED(CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL))) {
            CoUninitialize();
            return "Failed to initialize COM security";
        }

        IWbemLocator* pLoc = nullptr;
        if (FAILED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc))) {
            CoUninitialize();
            return "Failed to create IWbemLocator";
        }

        IWbemServices* pSvc = nullptr;
        if (FAILED(pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc))) {
            pLoc->Release();
            CoUninitialize();
            return "Failed to connect to WMI namespace";
        }

        if (FAILED(CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE))) {
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return "Failed to set proxy blanket";
        }

        IEnumWbemClassObject* pEnumerator = nullptr;
        if (FAILED(pSvc->ExecQuery(bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_Baseboard WHERE Tag = \"Base Board\""),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator))) {
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return "Query execution failed";
        }

        IWbemClassObject* pclsObj = nullptr;
        ULONG uReturn = 0;
        std::string data = "000-000-000";
        while (pEnumerator && SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn) {
            VARIANT vtProp;
            if (SUCCEEDED(pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0))) {
                data = WCharHelper::WCharToString(vtProp.bstrVal);
                VariantClear(&vtProp);
            }
            pclsObj->Release();
        }

        if (pEnumerator) pEnumerator->Release();
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();

        return data;
    }
    std::string GetSMBiosUUID() {
        DWORD smbiosSize = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
        if (smbiosSize == 0) return "Unable to retrieve SMBIOS table";

        std::vector<BYTE> smbiosData(smbiosSize);
        if (GetSystemFirmwareTable('RSMB', 0, smbiosData.data(), smbiosSize) == 0)
            return "Failed to read SMBIOS data";

        const BYTE* smbiosPtr = smbiosData.data();
        const DWORD length = smbiosSize;

        const size_t uuidOffset = 8;
        for (size_t i = 0; i < length;) {
            BYTE type = smbiosPtr[i];
            BYTE size = smbiosPtr[i + 1];

            if (type == 1 && size >= uuidOffset + 16) {
                const BYTE* uuid = smbiosPtr + i + uuidOffset;
                char uuidStr[37];
                snprintf(uuidStr, sizeof(uuidStr),
                    "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                    uuid[3], uuid[2], uuid[1], uuid[0], uuid[5], uuid[4], uuid[7], uuid[6],
                    uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);

                return std::string(uuidStr);
            }

            if (type == 127) break;
            i += size;
            while (i < length && smbiosPtr[i] != 0) ++i;
            ++i;
        }

        return "No UUID found.";
    }
    std::string GetGPU() {
        IDXGIFactory* factory = nullptr;
        if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
            return "";

        IDXGIAdapter* adapter = nullptr;
        std::string result;
        if (SUCCEEDED(factory->EnumAdapters(0, &adapter))) {
            DXGI_ADAPTER_DESC adapterdesc;
            if (SUCCEEDED(adapter->GetDesc(&adapterdesc))) {
                result = WCharHelper::WStringToConstChar(adapterdesc.Description);
            }
            adapter->Release();
        }
        factory->Release();
        return result;
    }
    size_t GetTotalMemory() {
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        if (!GlobalMemoryStatusEx(&status))
            return 1024;
        return status.ullTotalPhys / (static_cast<unsigned long long>(1024) * 1024);
    }
    std::vector<std::string> GetMacAddresses() {
        std::vector<std::string> ret;
        DWORD buffer = 0;

        if (GetAdaptersInfo(nullptr, &buffer) == ERROR_BUFFER_OVERFLOW) {
            PIP_ADAPTER_INFO adapterinfo = (PIP_ADAPTER_INFO)malloc(buffer);

            if (adapterinfo) {
                if (GetAdaptersInfo(adapterinfo, &buffer) == ERROR_SUCCESS) {
                    PIP_ADAPTER_INFO pipadapterinfo = adapterinfo;
                    while (pipadapterinfo) {
                        if (pipadapterinfo->AddressLength > 0) {
                            std::string mac;
                            for (int i = 0; i < pipadapterinfo->AddressLength; i++) {
                                char buffer[3];
                                if (i > 0) mac += ":";
                                sprintf_s(buffer, "%02X", pipadapterinfo->Address[i]);
                                mac += buffer;
                            }
                            if (!mac.empty()) ret.push_back(mac);
                        }
                        pipadapterinfo = pipadapterinfo->Next;
                    }
                }
                free(adapterinfo);
            }
        }

        return ret;
    }
    std::vector<std::string> GetDriveSerialNumbers()
    {
        std::vector<std::string> ret;
        int drivenumber = 0;

        while (true)
        {
            std::wstring path = L"\\\\.\\PhysicalDrive" + std::to_wstring(drivenumber);

            HANDLE h = CreateFileW(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
            if (h == INVALID_HANDLE_VALUE)
                break;

            std::unique_ptr<std::remove_pointer<HANDLE>::type, void(*)(HANDLE)> hDevice{ h, [](HANDLE handle) { CloseHandle(handle); } };

            STORAGE_PROPERTY_QUERY storagepropertyquery{};
            storagepropertyquery.PropertyId = StorageDeviceProperty;
            storagepropertyquery.QueryType = PropertyStandardQuery;

            STORAGE_DESCRIPTOR_HEADER storagedescriptorheader{};
            DWORD bytesret = 0;

            if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY,
                &storagepropertyquery, sizeof(STORAGE_PROPERTY_QUERY),
                &storagedescriptorheader, sizeof(STORAGE_DESCRIPTOR_HEADER),
                &bytesret, NULL))
            {
                drivenumber++;
                continue;
            }

            const DWORD buffersize = storagedescriptorheader.Size;
            std::unique_ptr<BYTE[]> buffer{ new BYTE[buffersize]{} };

            if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY,
                &storagepropertyquery, sizeof(STORAGE_PROPERTY_QUERY),
                buffer.get(), buffersize, &bytesret, NULL))
            {
                drivenumber++;
                continue;
            }

            STORAGE_DEVICE_DESCRIPTOR* devicedescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(buffer.get());
            const DWORD serialnumberoffset = devicedescriptor->SerialNumberOffset;

            if (serialnumberoffset != 0)
            {
                const char* serialnumber = reinterpret_cast<const char*>(buffer.get() + serialnumberoffset);
                ret.emplace_back(serialnumber);
            }
            drivenumber++;
        }

        return ret;
    }
    std::vector<std::string> GetDrivesLetters() {
        try {
            std::vector<std::string> drives;
            DWORD driveMask = GetLogicalDrives();
            for (char letter = 'A'; letter <= 'Z'; ++letter) {
                if (driveMask & 1) {
                    std::string drive(1, letter);
                    drive += ":\\";
                    drives.push_back(drive);
                }
                driveMask >>= 1;
            }
            return drives.empty() ? std::vector<std::string>{"No Drives Found"} : drives;
        }
        catch (...) {
            return { "No Drives Found" };
        }
    }
    std::string GetVolumeSerialNumber(const std::string& volume) {
        try {
            DWORD serialNumber = 0;
            if (GetVolumeInformationA(volume.c_str(), nullptr, 0, &serialNumber, nullptr, nullptr, nullptr, 0)) {
                if (serialNumber == 0) return "000-000";
                std::ostringstream ss;
                ss << std::hex << ((serialNumber >> 16) & 0xFFFF) << "-" << (serialNumber & 0xFFFF);
                return ss.str();
            }
            return "000-000";
        }
        catch (...) {
            return "000-000";
        }
    }
}