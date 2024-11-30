#pragma once
#include <string>
#include <vector>

class EXText {
public:
    static std::string encrypt(const std::string& text, const std::string& key);
    static std::string decrypt(const std::string& encryptedText, const std::string& key);
    static bool isEncrypted(const std::string& data);
private:
    static std::string gzipCompress(const std::string& data);
    static std::string gzipDecompress(const std::string& data);
    static std::string OperateXOR(const std::string& data, const std::string& key);
    static std::string AESEncrypt(const std::string& data, const std::string& key);
    static std::string AESDecrypt(const std::string& data, const std::string& key);
};