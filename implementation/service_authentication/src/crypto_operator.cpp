#include "../include/crypto_operator.hpp"
#include <sstream>
#include <iomanip>
#include <cryptopp/files.h>
#include <cryptopp/pem.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define DOUBLE_DIGIT 2
#define ZERO_FILL '0'

std::mutex crypto_operator::mutex_;
crypto_operator* crypto_operator::instance_;

crypto_operator::crypto_operator() {
    rng_.Reseed();
}

crypto_operator::~crypto_operator() {
}

crypto_operator* crypto_operator::get_instance() {
    std::lock_guard<std::mutex> lockguard(mutex_);
    if(instance_ == nullptr) {
        instance_ = new crypto_operator();
    }
    return instance_;
}

std::vector<CryptoPP::byte> crypto_operator::encrypt(CryptoPP::PublicKey& _public_key, std::vector<CryptoPP::byte> _data) {
    std::vector<CryptoPP::byte> encypted_data;
    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(_public_key);
    try {
        CryptoPP::VectorSource vectorSource(_data, true, new CryptoPP::PK_EncryptorFilter(rng_, encryptor, new CryptoPP::VectorSink(encypted_data)));
    } catch (std::exception& _exception) {
        std::cerr << _exception.what() << std::endl;
        encypted_data.clear();
    }
    return encypted_data;
}

std::vector<CryptoPP::byte> crypto_operator::decrypt(CryptoPP::PrivateKey& _private_key, std::vector<unsigned char> _data) {
    std::vector<CryptoPP::byte> decrypted_data;
    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(_private_key);
    CryptoPP::VectorSource vectorSource(_data, true, new CryptoPP::PK_DecryptorFilter(rng_, decryptor, new CryptoPP::VectorSink(decrypted_data)));
    return decrypted_data;
}

cfb_encrypted_data crypto_operator::encrypt(CryptoPP::SecByteBlock _symmetric_key, CryptoPP::SecByteBlock _data) {
    // Calculate a SHA-256 hash over the Diffie-Hellman session key
    CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);
    CryptoPP::SHA256().CalculateDigest(key, _symmetric_key, _symmetric_key.SizeInBytes());
    // Generate a random IV
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    rng_.GenerateBlock(iv, CryptoPP::AES::BLOCKSIZE);
    std::vector<CryptoPP::byte> iv_vector(iv, iv + CryptoPP::AES::BLOCKSIZE);
    // Encrypt
    CryptoPP::SecByteBlock encrypted_data(_data.SizeInBytes());
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(key, CryptoPP::SHA256::DIGESTSIZE, iv);
    cfbEncryption.ProcessData(encrypted_data.BytePtr(), _data.BytePtr(), _data.SizeInBytes());
    cfb_encrypted_data cfbencrypted_data;
    cfbencrypted_data.encrypted_data_ = encrypted_data;
    cfbencrypted_data.initialization_vector_ = iv_vector;
    return cfbencrypted_data;
}

CryptoPP::SecByteBlock crypto_operator::decrypt(CryptoPP::SecByteBlock _symmetric_key, cfb_encrypted_data _cfb_encrypted_data) {
    // Calculate a SHA-256 hash over the Diffie-Hellman session key
    CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);
    CryptoPP::SHA256().CalculateDigest(key, _symmetric_key, _symmetric_key.SizeInBytes());
    // Decrypt
    CryptoPP::SecByteBlock decrypted_data(_cfb_encrypted_data.encrypted_data_.SizeInBytes());
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(key, CryptoPP::SHA256::DIGESTSIZE, _cfb_encrypted_data.initialization_vector_.data());
    cfbDecryption.ProcessData(decrypted_data.BytePtr(), _cfb_encrypted_data.encrypted_data_.BytePtr(), _cfb_encrypted_data.encrypted_data_.SizeInBytes());
    return decrypted_data;
}

std::vector<CryptoPP::byte> crypto_operator::sign(CryptoPP::PrivateKey& _private_key, std::vector<CryptoPP::byte> _data) {
    std::vector<CryptoPP::byte> signature;
    CryptoPP::RSASSA_PKCS1v15_SHA_Signer signer(_private_key);
    CryptoPP::VectorSource vectorSource(_data, true, new CryptoPP::SignerFilter(rng_, signer, new CryptoPP::VectorSink(signature)));
    return signature;
}

bool crypto_operator::verify(CryptoPP::PublicKey& _public_key, std::vector<CryptoPP::byte> _data) {
    bool verified = false;

    CryptoPP::RSASSA_PKCS1v15_SHA_Verifier verifier(_public_key);
    try {
        CryptoPP::VectorSource vectorSource(_data, true, new CryptoPP::SignatureVerificationFilter( verifier, NULL, 
                                                            CryptoPP::SignatureVerificationFilter::THROW_EXCEPTION));
        //std::cout << "Verified signature on data" << std::endl;
        verified = true;
    } catch (CryptoPP::SignatureVerificationFilter::SignatureVerificationFailed& _exception) {
        std::cerr << _exception.GetWhat() << std::endl;
    }
    return verified;
}

std::string crypto_operator::convert_hex_byte_vector_to_hex_string(std::vector<CryptoPP::byte> _data) {
    std::stringstream ss;
    for (auto it = _data.begin(); it != _data.end(); it++) {
        ss << std::hex << std::setw(DOUBLE_DIGIT) << std::setfill(ZERO_FILL) << (int)*it;
    }
    return ss.str();
}

void crypto_operator::load(const std::string& _filename, CryptoPP::BufferedTransformation& _bt) {
    CryptoPP::FileSource file(_filename.c_str(), true);
    file.TransferTo(_bt);
    _bt.MessageEnd();
}

void crypto_operator::load_private_key(const std::string& _filename, CryptoPP::PrivateKey& _key) {
    CryptoPP::ByteQueue queue;
    load(_filename, queue);
    _key.Load(queue);    
}

void crypto_operator::load_pem_private_key(const std::string& _filename, CryptoPP::RSA::PrivateKey& _key) {
    CryptoPP::FileSource file_source(_filename.c_str(), true);
    CryptoPP::PEM_Load(file_source, _key);
}

void crypto_operator::load_certificate_from_file(const std::string& _filename, CryptoPP::X509Certificate& _certificate) {
    CryptoPP::FileSource fs(_filename.c_str(), true);
    CryptoPP::PEM_Load(fs, _certificate);
}

std::vector<CryptoPP::byte> crypto_operator::load_certificate_from_file(const std::string& _filename) {
    std::vector<CryptoPP::byte> _certificate;
    CryptoPP::FileSource fs(_filename.c_str(), true, new CryptoPP::VectorSink(_certificate));
    return _certificate;
}

void crypto_operator::load_certificate_from_string(const std::string& _certificate_string, CryptoPP::X509Certificate& _certificate) {
    CryptoPP::StringSource ss(_certificate_string, true);
    CryptoPP::PEM_Load(ss, _certificate);
}

bool crypto_operator::extract_public_key_from_certificate(std::vector<CryptoPP::byte> _certificate_data, CryptoPP::RSA::PublicKey& _public_key)
{
    // Load certificate from string
    CryptoPP::X509Certificate certificate;
    CryptoPP::VectorSource vs(_certificate_data, true);
    CryptoPP::PEM_Load(vs, certificate);
    const CryptoPP::SecByteBlock& signature = certificate.GetCertificateSignature();
    const CryptoPP::SecByteBlock& to_be_signed = certificate.GetToBeSigned();
    const CryptoPP::X509PublicKey& public_key = certificate.GetSubjectPublicKey();
    // Verify self signed certificate
    CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA256>::Verifier verifier(public_key);
    bool result = verifier.VerifyMessage(to_be_signed, to_be_signed.size(), signature, signature.size());
    _public_key = verifier.AccessKey();
    return result;
}

using BIO_MEM_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;
std::vector<CryptoPP::byte> crypto_operator::convert_pem_to_der(const std::vector<CryptoPP::byte> _pem_certificate) {
    // Load certificate into BIO data type
    BIO_MEM_ptr initial_bio(BIO_new(BIO_s_mem()), ::BIO_free);
    BIO_puts(initial_bio.get(), (char*)_pem_certificate.data());

    // Load certificate from BIO into X509 data type
    int rc = 0;
    unsigned long err = 0;
    X509* x509 = NULL;
    x509 = PEM_read_bio_X509(initial_bio.get(), NULL, NULL, NULL);
    // Load certificate from X509 into BIO data type
    BIO_MEM_ptr pem_bio(BIO_new(BIO_s_mem()), ::BIO_free);
    rc = PEM_write_bio_X509(pem_bio.get(), x509);
    err = ERR_get_error();
    if (rc != 1)
    {
        std::cerr << "PEM_write_bio_X509 failed, error " << err << ", ";
        std::cerr << std::hex << "0x" << err;
        exit(1);
    }

    // Load certificate from BIO into memory buffer
    BUF_MEM *mem = NULL;
    BIO_get_mem_ptr(pem_bio.get(), &mem);
    err = ERR_get_error();
    if (!mem || !mem->data || !mem->length)
    {
        std::cerr << "BIO_get_mem_ptr failed, error " << err << ", ";
        std::cerr << std::hex << "0x" << err;
        exit(2);
    }

    // Convert from PEM to DER and load it into BIO data type
    BIO_MEM_ptr der_bio(BIO_new(BIO_s_mem()), ::BIO_free);
    i2d_X509_bio(der_bio.get(), x509);
    // Load DER certificate from BIO into memory buffer
    BUF_MEM *der_mem = NULL;
    BIO_get_mem_ptr(der_bio.get(), &der_mem);
    err = ERR_get_error();
    if (!der_mem || !der_mem->data || !der_mem->length)
    {
        std::cerr << "BIO_get_mem_ptr failed, error " << err << ", ";
        std::cerr << std::hex << "0x" << err;
        exit(2);
    }
    return std::vector<CryptoPP::byte>(der_mem->data, der_mem->data+der_mem->length);
}

std::vector<CryptoPP::byte> crypto_operator::convert_der_to_pem(const std::vector<CryptoPP::byte> der_certificate) {
    int rc = 0;
    unsigned long err = 0;
    X509* pem_certificate;
    const unsigned char* p = der_certificate.data();
    pem_certificate = d2i_X509(NULL, &p, der_certificate.size());
    BIO_MEM_ptr pem_bio_ptr(BIO_new(BIO_s_mem()), ::BIO_free);
    rc = PEM_write_bio_X509(pem_bio_ptr.get(), pem_certificate);
    err = ERR_get_error();
    if (rc != 1)
    {
        std::cerr << "PEM_write_bio_X509 failed, error " << err << ", ";
        std::cerr << std::hex << "0x" << err;
        exit(1);
    }

    // Load certificate from BIO into memory buffer
    BUF_MEM *pem_memory = NULL;
    BIO_get_mem_ptr(pem_bio_ptr.get(), &pem_memory);
    err = ERR_get_error();
    if (!pem_memory || !pem_memory->data || !pem_memory->length)
    {
        std::cerr << "BIO_get_mem_ptr failed, error " << err << ", ";
        std::cerr << std::hex << "0x" << err;
        exit(2);
    }
    return std::vector<CryptoPP::byte>(pem_memory->data, pem_memory->data+pem_memory->length);
}

CryptoPP::word32 crypto_operator::get_random_word_32() {
    return rng_.GenerateWord32();
}

CryptoPP::SecByteBlock crypto_operator::get_random_byte_block(int _blocksize) {
    CryptoPP::SecByteBlock random_byte_block(_blocksize);
    rng_.GenerateBlock(random_byte_block, random_byte_block.size());
    return random_byte_block;
}

std::vector<CryptoPP::byte> crypto_operator::convert_string_to_byte_vector(const std::string& _string_to_convert) {
    std::vector<CryptoPP::byte> byte_vector;
    const char* certificate_char_array = _string_to_convert.c_str();
    byte_vector.assign(certificate_char_array, certificate_char_array+strlen(certificate_char_array));
    return byte_vector;
}

std::string crypto_operator::convert_byte_vector_to_string(std::vector<CryptoPP::byte> _vector_to_convert) {
    return std::string(_vector_to_convert.begin(), _vector_to_convert.end());
}

std::vector<CryptoPP::byte> crypto_operator::hex_encode(std::vector<CryptoPP::byte> _data) {
    std::vector<CryptoPP::byte> hex_encoded;
    CryptoPP::VectorSource vector_source(_data, true, new CryptoPP::HexEncoder(new CryptoPP::VectorSink(hex_encoded)));
    return hex_encoded;
}

std::vector<CryptoPP::byte> crypto_operator::hex_decode(std::vector<CryptoPP::byte> _data) {
    std::vector<CryptoPP::byte> hex_decoded;
    CryptoPP::VectorSource vector_source(_data, true, new CryptoPP::HexDecoder(new CryptoPP::VectorSink(hex_decoded)));
    return hex_decoded;
}
