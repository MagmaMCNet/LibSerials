#include "HardwareInfo.h"

std::string HardwareInfo::GetCPU() {
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

std::string HardwareInfo::GetCPUSerialNumber() {
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

std::string HardwareInfo::GetBiosSerialNumber() {
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

std::string HardwareInfo::GetSMBiosUUID() {
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

std::string HardwareInfo::GetGPU() {
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

size_t HardwareInfo::GetTotalMemory() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (!GlobalMemoryStatusEx(&status))
        return 1024;
    return status.ullTotalPhys / (static_cast<unsigned long long>(1024) * 1024);
}

std::vector<std::string> HardwareInfo::GetMacAddresses() {
    std::vector<std::string> ret;
    DWORD buffer = 0;

    if (GetAdaptersInfo(NULL, &buffer) == ERROR_BUFFER_OVERFLOW) {
        PIP_ADAPTER_INFO adapterinfo = (PIP_ADAPTER_INFO)malloc(buffer);

        if (adapterinfo) {
            if (GetAdaptersInfo(adapterinfo, &buffer) == ERROR_SUCCESS) {
                PIP_ADAPTER_INFO pipadapterinfo = adapterinfo;
                while (pipadapterinfo) {
                    std::string mac;
                    for (int i = 0; i < pipadapterinfo->AddressLength; i++) {
                        char buffer[3];
                        if (i > 0) mac += ":";
                        sprintf_s(buffer, "%02X", pipadapterinfo->Address[i]);
                        mac += buffer;
                    }
                    ret.push_back(mac);
                    pipadapterinfo = pipadapterinfo->Next;
                }
            }
            free(adapterinfo);
        }
    }

    return ret;
}

std::vector<std::string> HardwareInfo::GetDriveSerialNumbers()
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
std::vector<std::string> HardwareInfo::GetDrivesLetters() {
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
std::string HardwareInfo::GetVolumeSerialNumber(const std::string& volume) {
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