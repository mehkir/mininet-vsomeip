#ifndef VSOMEIP_CRYPTO_OPERATOR_HPP
#define VSOMEIP_CRYPTO_OPERATOR_HPP

#include <mutex>
#include <cryptopp/osrng.h>
#include <cryptopp/x509cert.h>

class crypto_operator
{
private:
    crypto_operator();
    ~crypto_operator();
    static std::mutex mutex_;
    static crypto_operator* instance;
    CryptoPP::AutoSeededRandomPool rng;
    void crypto_operator::Load(const std::string& filename, CryptoPP::BufferedTransformation& bt);
public:
    static crypto_operator* getInstance();
    std::vector<CryptoPP::byte> encrypt(CryptoPP::PublicKey &publicKey, std::vector<CryptoPP::byte> data);
    std::vector<CryptoPP::byte> decrypt(CryptoPP::PrivateKey &privateKey, std::vector<CryptoPP::byte> data);
    std::string convertHexByteVectorToHexString(std::vector<CryptoPP::byte> data);
    void loadCertificateFromFile(const std::string& filename, CryptoPP::X509Certificate& certificate);
    std::string loadCertificateFromFile(const std::string& filename);
    void loadCertificateFromString(const std::string& certificateString, CryptoPP::X509Certificate& certificate);
    void LoadPrivateKey(const std::string& filename, CryptoPP::PrivateKey& key);
    void extractPublicKeyFromCertificate(CryptoPP::X509Certificate& certificate, CryptoPP::X509PublicKey& publicKey);
    std::string convertPEMStringToDERString(const std::string& certificateString);
};

#endif /* VSOMEIP_CRYPTO_OPERATOR_HPP */
