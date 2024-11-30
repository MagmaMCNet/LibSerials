#include "EXText.h"
#include <zlib.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <stdexcept>
#include <sstream>

std::string EXText::gzipCompress(const std::string& data) {
    z_stream zs{};
    deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY);

    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));
    zs.avail_in = data.size();

    std::vector<char> buffer(128 * 1024);
    std::ostringstream out;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(buffer.data());
        zs.avail_out = buffer.size();
        deflate(&zs, Z_FINISH);
        out.write(buffer.data(), buffer.size() - zs.avail_out);
    } while (zs.avail_out == 0);

    deflateEnd(&zs);
    return out.str();
}
std::string EXText::gzipDecompress(const std::string& data) {
    z_stream zs{};
    inflateInit2(&zs, 15 | 16);

    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));
    zs.avail_in = data.size();

    std::vector<char> buffer(128 * 1024);
    std::ostringstream out;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(buffer.data());
        zs.avail_out = buffer.size();
        inflate(&zs, Z_NO_FLUSH);
        out.write(buffer.data(), buffer.size() - zs.avail_out);
    } while (zs.avail_out == 0);

    inflateEnd(&zs);
    return out.str();
}

std::string EXText::OperateXOR(const std::string& data, const std::string& key) {
    std::string result = data;
    for (size_t i = 0; i < data.size(); ++i)
        result[i] ^= key[i % key.size()];
    return result;
}

std::string EXText::aesEncrypt(const std::string& data, const std::string& key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create AES context");

    unsigned char iv[AES_BLOCK_SIZE] = { 0 }; // Initialize IV with zeroes
    std::vector<unsigned char> encrypted(data.size() + AES_BLOCK_SIZE);
    int len;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.data()), iv);
    EVP_EncryptUpdate(ctx, encrypted.data(), &len, reinterpret_cast<const unsigned char*>(data.data()), data.size());
    int totalLen = len;
    EVP_EncryptFinal_ex(ctx, encrypted.data() + len, &len);
    totalLen += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(encrypted.data()), totalLen);
}
std::string EXText::aesDecrypt(const std::string& data, const std::string& key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create AES context");

    unsigned char iv[AES_BLOCK_SIZE] = { 0 };
    std::vector<unsigned char> decrypted(data.size());
    int len;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.data()), iv);
    EVP_DecryptUpdate(ctx, decrypted.data(), &len, reinterpret_cast<const unsigned char*>(data.data()), data.size());
    int totalLen = len;
    EVP_DecryptFinal_ex(ctx, decrypted.data() + len, &len);
    totalLen += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(decrypted.data()), totalLen);
}

std::string EXText::encrypt(const std::string& Text, const std::string& Key) {
    auto Compressed = gzipCompress(Text);
    auto AESData = aesEncrypt(Compressed, Key);
    return OperateXOR(AESData, Key);
}
std::string EXText::decrypt(const std::string& EncryptedText, const std::string& Key) {
    auto XORData = OperateXOR(EncryptedText, Key);
    auto DecryptedAES = aesDecrypt(XORData, Key);
    return gzipDecompress(DecryptedAES);
}
