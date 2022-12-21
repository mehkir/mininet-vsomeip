#include "../include/crypto_operator.hpp"
#include <cryptopp/rsa.h>
#include <sstream>
#include <iomanip>
#include <cryptopp/files.h>
#include <cryptopp/pem.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define DOUBLE_DIGIT 2
#define ZERO_FILL '0'

std::mutex crypto_operator::mutex_;
crypto_operator* crypto_operator::instance;

crypto_operator::crypto_operator() {
    rng.Reseed();
}

crypto_operator::~crypto_operator() {
}

crypto_operator* crypto_operator::getInstance() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if(instance == nullptr) {
        instance = new crypto_operator();
    }
    return instance;
}

std::vector<CryptoPP::byte> crypto_operator::encrypt(CryptoPP::PublicKey &publicKey, std::vector<CryptoPP::byte> data) {
    std::vector<CryptoPP::byte> encryptedData;
    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
    try {
        CryptoPP::VectorSource vectorSource(data, true, new CryptoPP::PK_EncryptorFilter(rng, encryptor, new CryptoPP::VectorSink(encryptedData)));
    } catch (std::exception exception) {
        std::cerr << exception.what() << std::endl;
        encryptedData.clear();
    }
    return encryptedData;
}

std::vector<CryptoPP::byte> crypto_operator::decrypt(CryptoPP::PrivateKey &privateKey, std::vector<unsigned char> data) {
    std::vector<CryptoPP::byte> decryptedData;
    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(privateKey);
    CryptoPP::VectorSource vectorSource(data, true, new CryptoPP::PK_DecryptorFilter(rng, decryptor, new CryptoPP::VectorSink(decryptedData)));
    return decryptedData;
}

std::vector<CryptoPP::byte> crypto_operator::sign(CryptoPP::PrivateKey &privateKey, std::vector<CryptoPP::byte> data) {
    std::vector<CryptoPP::byte> signature;
    CryptoPP::RSASSA_PKCS1v15_SHA_Signer signer(privateKey);
    CryptoPP::VectorSource vectorSource(data, true, new CryptoPP::SignerFilter(rng, signer, new CryptoPP::VectorSink(signature)));
    return signature;
}

bool crypto_operator::verify(CryptoPP::PublicKey &publicKey, std::vector<CryptoPP::byte> data) {
    bool verified = false;

    CryptoPP::RSASSA_PKCS1v15_SHA_Verifier verifier(publicKey);
    try {
        CryptoPP::VectorSource vectorSource(data, true, new CryptoPP::SignatureVerificationFilter( verifier, NULL, 
                                                            CryptoPP::SignatureVerificationFilter::THROW_EXCEPTION));
        //std::cout << "Verified signature on data" << std::endl;
        verified = true;
    } catch (CryptoPP::SignatureVerificationFilter::SignatureVerificationFailed exception) {
        std::cout << exception.GetWhat() << std::endl;
    }
    return verified;
}

std::string crypto_operator::convertHexByteVectorToHexString(std::vector<CryptoPP::byte> data) {
    std::stringstream ss;
    for (auto it = data.begin(); it != data.end(); it++) {
        ss << std::hex << std::setw(DOUBLE_DIGIT) << std::setfill(ZERO_FILL) << (int)*it;
    }
    return ss.str();
}

void crypto_operator::Load(const std::string& filename, CryptoPP::BufferedTransformation& bt) {
    CryptoPP::FileSource file(filename.c_str(), true);
    file.TransferTo(bt);
    bt.MessageEnd();
}

void crypto_operator::LoadPrivateKey(const std::string& filename, CryptoPP::PrivateKey& key) {
    CryptoPP::ByteQueue queue;
    Load(filename, queue);
    key.Load(queue);    
}

void crypto_operator::loadCertificateFromFile(const std::string& filename, CryptoPP::X509Certificate& certificate) {
    CryptoPP::FileSource fs(filename.c_str(), true);
    CryptoPP::PEM_Load(fs, certificate);
}

std::string crypto_operator::loadCertificateFromFile(const std::string& filename) {
    std::string certificateString;
    CryptoPP::FileSource fs(filename.c_str(), true, new CryptoPP::StringSink(certificateString));
    return certificateString;
}

void crypto_operator::loadCertificateFromString(const std::string& certificateString, CryptoPP::X509Certificate& certificate) {
    CryptoPP::StringSource ss(certificateString, true);
    CryptoPP::PEM_Load(ss, certificate);
}

void crypto_operator::extractPublicKeyFromCertificate(CryptoPP::X509Certificate &certificate, CryptoPP::X509PublicKey &publicKey)
{
    publicKey = certificate.GetSubjectPublicKey();
}

using BIO_MEM_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;
std::string crypto_operator::convertPEMStringToDERString(const std::string& certificateString) {
    // Load certificate into BIO data type
    BIO_MEM_ptr initialBio(BIO_new(BIO_s_mem()), ::BIO_free);
    BIO_puts(initialBio.get(), certificateString.c_str());

    // Load certificate from BIO into X509 data type
    int rc = 0;
    unsigned long err = 0;
    X509* x509 = NULL;
    x509 = PEM_read_bio_X509(initialBio.get(), NULL, NULL, NULL);
    // Load certificate from X509 into BIO data type
    BIO_MEM_ptr pemBio(BIO_new(BIO_s_mem()), ::BIO_free);
    rc = PEM_write_bio_X509(pemBio.get(), x509);
    err = ERR_get_error();
    if (rc != 1)
    {
        std::cerr << "PEM_write_bio_X509 failed, error " << err << ", ";
        std::cerr << std::hex << "0x" << err;
        exit(1);
    }

    // Load certificate from BIO into memory buffer
    BUF_MEM *mem = NULL;
    BIO_get_mem_ptr(pemBio.get(), &mem);
    err = ERR_get_error();
    if (!mem || !mem->data || !mem->length)
    {
        std::cerr << "BIO_get_mem_ptr failed, error " << err << ", ";
        std::cerr << std::hex << "0x" << err;
        exit(2);
    }

    // Convert from PEM to DER and load it into BIO data type
    BIO_MEM_ptr derBio(BIO_new(BIO_s_mem()), ::BIO_free);
    i2d_X509_bio(derBio.get(), x509);
    // Load DER certificate from BIO into memory buffer
    BUF_MEM *derMem = NULL;
    BIO_get_mem_ptr(derBio.get(), &derMem);
    err = ERR_get_error();
    if (!derMem || !derMem->data || !derMem->length)
    {
        std::cerr << "BIO_get_mem_ptr failed, error " << err << ", ";
        std::cerr << std::hex << "0x" << err;
        exit(2);
    }
    return std::string(derMem->data, derMem->length);
}

CryptoPP::word32 crypto_operator::getRandomWord32() {
    return rng.GenerateWord32();
}

std::vector<CryptoPP::byte> crypto_operator::convertStringToByteVector(const std::string& stringToConvert) {
    std::vector<CryptoPP::byte> byteVector;
    const char* certificateCharArray = stringToConvert.c_str();
    byteVector.assign(certificateCharArray, certificateCharArray+strlen(certificateCharArray));
    return byteVector;
}

std::string crypto_operator::convertByteVectorToString(std::vector<CryptoPP::byte> vectorToConvert) {
    return std::string((char*)vectorToConvert.data(), 0, vectorToConvert.size());
}

