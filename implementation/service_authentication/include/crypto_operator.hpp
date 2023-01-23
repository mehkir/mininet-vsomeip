#ifndef VSOMEIP_CRYPTO_OPERATOR_HPP
#define VSOMEIP_CRYPTO_OPERATOR_HPP

#define DEFAULT_BYTE_BLOCKSIZE 4

#include <mutex>
#include <cryptopp/osrng.h>
#include <cryptopp/x509cert.h>
#include <cryptopp/rsa.h>

class crypto_operator
{
private:
    crypto_operator();
    ~crypto_operator();
    static std::mutex mutex_;
    static crypto_operator* instance;
    CryptoPP::AutoSeededRandomPool rng;
    void Load(const std::string& filename, CryptoPP::BufferedTransformation& bt);
public:
    static crypto_operator* getInstance();
    std::vector<CryptoPP::byte> encrypt(CryptoPP::PublicKey &publicKey, std::vector<CryptoPP::byte> data);
    std::vector<CryptoPP::byte> decrypt(CryptoPP::PrivateKey &privateKey, std::vector<CryptoPP::byte> data);
    std::vector<CryptoPP::byte> sign(CryptoPP::PrivateKey &privateKey, std::vector<CryptoPP::byte> data);
    bool verify(CryptoPP::PublicKey &publicKey, std::vector<CryptoPP::byte> data);
    std::string convertHexByteVectorToHexString(std::vector<CryptoPP::byte> data);
    void loadCertificateFromFile(const std::string& filename, CryptoPP::X509Certificate& certificate);
    std::vector<CryptoPP::byte> loadCertificateFromFile(const std::string& filename);
    void loadCertificateFromString(const std::string& certificateString, CryptoPP::X509Certificate& certificate);
    void LoadPrivateKey(const std::string& filename, CryptoPP::PrivateKey& key);
    void LoadPEMPrivateKey(const std::string& filename, CryptoPP::RSA::PrivateKey& key);
    bool extractPublicKeyFromCertificate(std::vector<CryptoPP::byte> certificate_data, CryptoPP::RSA::PublicKey& public_key);
    std::vector<CryptoPP::byte> convertPEMToDER(const std::vector<CryptoPP::byte> pem_certificate);
    std::vector<CryptoPP::byte> convertDERToPEM(const std::vector<CryptoPP::byte> der_certificate);
    CryptoPP::word32 getRandomWord32();
    CryptoPP::SecByteBlock getRandomByteBlock(int blocksize=DEFAULT_BYTE_BLOCKSIZE);
    std::vector<CryptoPP::byte> convertStringToByteVector(const std::string& stringToConvert);
    std::string convertByteVectorToString(std::vector<CryptoPP::byte> vectorToConvert);
    std::vector<CryptoPP::byte> hex_encode(std::vector<CryptoPP::byte> data);
    std::vector<CryptoPP::byte> hex_decode(std::vector<CryptoPP::byte> data);
};

#endif /* VSOMEIP_CRYPTO_OPERATOR_HPP */
