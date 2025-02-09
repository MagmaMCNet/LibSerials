# LibSerials

**Open Source Library Used For Getting System Information From SMBIOS Table**

### Current Supported Serials:
- [x] HDD Serial Numbers
- [x] BIOS Information
- [x] System Information
- [x] Baseboard Information
- [x] Chassis Information
- [x] CPU Serial Number
- [x] MAC Addresses
- [x] HWID String
- [x] HWID Validation

## Example Serials Using The Built in Debug UI
![image](https://github.com/user-attachments/assets/eb2cbab6-edcf-489e-b0a9-07dde9daaee8)

## Example Usage
```cpp
void main() {
    Initialize();
    std::cout << " [!] Motherboard" << std::endl;
    std::cout << "[Manufacturer] " << Baseboard_Manufacturer() << std::endl;
    std::cout << "[Product]      " << Baseboard_Product() << std::endl;
    std::cout << "[Serial]       " << Baseboard_Serial() << std::endl;
    FreeSerials();
    std::cin.ignore();
}
```

## Functions

### Hardware Information Functions

#### HDD Information
- `const std::vector<std::string> HDD_Serials();`
  - Retrieves a list of serial numbers for all detected hard disk drives.

#### Baseboard Information
- `const std::string Baseboard_Manufacturer();`
  - Returns the manufacturer of the system's baseboard.
- `const std::string Baseboard_Product();`
  - Returns the product name of the baseboard.
- `const std::string Baseboard_Serial();`
  - Returns the serial number of the baseboard.

#### BIOS Information
- `const std::string BIOS_Vendor();`
  - Returns the vendor name of the system BIOS.
- `const std::string BIOS_Version();`
  - Returns the version of the system BIOS.
- `const std::string BIOS_Date();`
  - Returns the release date of the system BIOS.

#### Chassis Information
- `const std::string Chassis_Manufacturer();`
  - Returns the manufacturer of the chassis.
- `const std::string Chassis_Version();`
  - Returns the version of the chassis.
- `const std::string Chassis_Serial();`
  - Returns the serial number of the chassis.

#### System Information
- `const std::string System_Manufacturer();`
  - Returns the manufacturer of the system.
- `const std::string System_Product();`
  - Returns the product name of the system.
- `const std::string System_Version();`
  - Returns the version of the system.
- `const std::string System_Serial();`
  - Returns the serial number of the system.
- `const std::string System_SKU();`
  - Returns the SKU (Stock Keeping Unit) identifier of the system.
- `const std::string System_Family();`
  - Returns the family type of the system.
- `const std::string System_UUID();`
  - Returns the UUID (Universally Unique Identifier) of the system.

#### CPU Information
- `const std::string CPU_Product();`
  - Returns the product name of the CPU.
- `const std::string CPU_Serial();`
  - Returns the serial number of the CPU.

#### Network Information
- `const std::vector<std::string> MACAddress_List();`
  - Returns a list of all detected MAC addresses.
- `const std::string MACAddress_Main();`
  - Returns the primary MAC address of the system.

## Contribution
Feel free to contribute to the project by submitting pull requests, reporting issues, or suggesting new features.
