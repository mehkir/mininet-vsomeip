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
    static crypto_operator* instance_;
    CryptoPP::AutoSeededRandomPool rng_;
    void load(const std::string& _filename, CryptoPP::BufferedTransformation& _bt);
public:
    static crypto_operator* get_instance();
    std::vector<CryptoPP::byte> encrypt(CryptoPP::PublicKey& _public_key, std::vector<CryptoPP::byte> _data);
    std::vector<CryptoPP::byte> decrypt(CryptoPP::PrivateKey& _private_key, std::vector<CryptoPP::byte> _data);
    std::vector<CryptoPP::byte> sign(CryptoPP::PrivateKey& _private_key, std::vector<CryptoPP::byte> _data);
    bool verify(CryptoPP::PublicKey& _public_key, std::vector<CryptoPP::byte> _data);
    std::string convert_hex_byte_vector_to_hex_string(std::vector<CryptoPP::byte> _data);
    void load_certificate_from_file(const std::string& _filename, CryptoPP::X509Certificate& _certificate);
    std::vector<CryptoPP::byte> load_certificate_from_file(const std::string& _filename);
    void load_certificate_from_string(const std::string& _certificate_string, CryptoPP::X509Certificate& _certificate);
    void load_private_key(const std::string& _filename, CryptoPP::PrivateKey& _key);
    void load_pem_private_key(const std::string& _filename, CryptoPP::RSA::PrivateKey& _key);
    bool extract_public_key_from_certificate(std::vector<CryptoPP::byte> _certificate_data, CryptoPP::RSA::PublicKey& _public_key);
    std::vector<CryptoPP::byte> convert_pem_to_der(const std::vector<CryptoPP::byte> _pem_certificate);
    std::vector<CryptoPP::byte> convert_der_to_pem(const std::vector<CryptoPP::byte> _der_certificate);
    CryptoPP::word32 get_random_word_32();
    CryptoPP::SecByteBlock get_random_byte_block(int _blocksize=DEFAULT_BYTE_BLOCKSIZE);
    std::vector<CryptoPP::byte> convert_string_to_byte_vector(const std::string& _string_to_convert);
    std::string convert_byte_vector_to_string(std::vector<CryptoPP::byte> _vector_to_convert);
    std::vector<CryptoPP::byte> hex_encode(std::vector<CryptoPP::byte> _data);
    std::vector<CryptoPP::byte> hex_decode(std::vector<CryptoPP::byte> _data);
};

#endif /* VSOMEIP_CRYPTO_OPERATOR_HPP */
