#pragma once

struct SMBIOSStruct {
    BYTE    Type;
    BYTE    Length;
    WORD    Handle;
};
struct SMBIOSBaseBoard : SMBIOSStruct {
    BYTE    Manufacturer;
    BYTE    Product;
    BYTE    Version;
    BYTE    SerialNumber;
    BYTE    AssetTag;
    BYTE    FeatureFlags;
    BYTE    LocationInChassis;
    WORD    ChassisHandle;
    BYTE    BoardType;
    BYTE    NumberOfContainedObjectHandles;
    BYTE    ContainedObjectHandles[256];
};
struct SMBIOSPhysicalMemory : SMBIOSStruct {
    WORD    PhysicalMemoryArrayHandle;
    WORD    MemoryErrorInformationHandle;
    WORD    TotalWidth;
    WORD    DataWidth;
    WORD    Size;
    BYTE    FormFactor;
    BYTE    DeviceSet;
    BYTE    DeviceLocator;
    BYTE    BankLocator;
    BYTE    MemoryType;
    WORD    TypeDetail;
    WORD    Speed;
    BYTE    Manufacturer;
    BYTE    SerialNumber;
    BYTE    AssetTag;
    BYTE    PartNumber;
    BYTE    Attributes;
    BYTE    ExtendedSize[4];
    WORD    ConfiguredMemoryClockSpeed;
    WORD    MinimumVoltage;
    WORD    MaximumVoltage;
    WORD    ConfiguredVoltage;
};
struct RawSMBIOSData {
    BYTE    Used20CallingMethod;
    BYTE    SMBIOSMajorVersion;
    BYTE    SMBIOSMinorVersion;
    BYTE    DmiRevision;
    DWORD   Length;
    BYTE    SMBIOSTableData[];
};

struct SMBIOSBIOSInfo : SMBIOSStruct {
    BYTE Vendor;
    BYTE Version;
    BYTE ReleaseDate;
};

struct SMBIOSSystemInfo : SMBIOSStruct {
    BYTE Manufacturer;
    BYTE ProductName;
    BYTE Version;
    BYTE SerialNumber;
    int8_t UUID[16];
    BYTE Family;
    BYTE SKUNum;
};

struct SMBIOSChassis : SMBIOSStruct {
    uint8_t Type;
    uint8_t Length;
    uint16_t Handle;
    uint8_t Manufacturer;
    uint8_t TypeCode;
    uint8_t Version;
    uint8_t SerialNumber;
    uint8_t AssetTagNumber;
    uint8_t BootupState;
    uint8_t PowerSupplyState;
    uint8_t ThermalState;
    uint8_t SecurityStatus;
};

struct SMBIOSProcessorInfo : SMBIOSStruct {
    uint8_t Type;
    uint8_t Length;
    uint16_t Handle;
    uint8_t SocketDesignation;
    uint8_t ProcessorType;
    uint8_t ProcessorFamily;
    uint8_t Manufacturer;
    uint64_t ProcessorID;
    uint8_t Version;
    uint8_t Voltage;
    uint16_t ExternalClock;
    uint16_t MaxSpeed;
    uint16_t CurrentSpeed;
    uint8_t Status;
    uint8_t ProcessorUpgrade;
    uint16_t L1CacheHandle;
    uint16_t L2CacheHandle;
    uint16_t L3CacheHandle;
    uint8_t SerialNumber;
    uint8_t AssetTag;
    uint8_t PartNumber;
};
