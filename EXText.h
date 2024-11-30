#ifndef EXTEXT_H
#define EXTEXT_H

#include <string>
#include <vector>

class EXText {
public:
    static std::string encrypt(const std::string& text, const std::string& key);
    static std::string decrypt(const std::string& encryptedText, const std::string& key);

private:
    static std::string gzipCompress(const std::string& data);
    static std::string gzipDecompress(const std::string& data);
    static std::string OperateXOR(const std::string& data, const std::string& key);
    static std::string aesEncrypt(const std::string& data, const std::string& key);
    static std::string aesDecrypt(const std::string& data, const std::string& key);
};

#endif
