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
    BYTE Manufacturer;
    BYTE TypeCode;
    BYTE Version;
    BYTE SerialNumber;
    BYTE AssetTagNumber;
    BYTE BootupState;
    BYTE PowerSupplyState;
    BYTE ThermalState;
    BYTE SecurityStatus;
};

struct SMBIOSProcessorInfo : SMBIOSStruct {
    BYTE SocketDesignation;
    BYTE ProcessorType;
    BYTE ProcessorFamily;
    BYTE Manufacturer;
    BYTE ProcessorID;
    BYTE Version;
    BYTE Voltage;
    BYTE ExternalClock;
    BYTE MaxSpeed;
    BYTE CurrentSpeed;
    BYTE Status;
    BYTE ProcessorUpgrade;
    BYTE L1CacheHandle;
    BYTE L2CacheHandle;
    BYTE L3CacheHandle;
    BYTE SerialNumber;
    BYTE AssetTag;
    BYTE PartNumber;
};
