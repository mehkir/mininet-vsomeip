#ifndef VSOMEIP_DH_ECC_HPP
#define VSOMEIP_DH_ECC_HPP

#include <cryptopp/dh.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>

struct encrypted_group_secret_result {
    CryptoPP::SecByteBlock blinded_publisher_secret_;
    CryptoPP::SecByteBlock encrypted_group_secret_;
    std::vector<unsigned char> initialization_vector_;
};

class dh_ecc {
    public:
        dh_ecc();
        ~dh_ecc();
        encrypted_group_secret_result compute_encrypted_group_secret(CryptoPP::SecByteBlock _blinded_subscriber_secret);
        CryptoPP::SecByteBlock decrypt_group_secret(encrypted_group_secret_result _encrypted_group_secret_result);
    protected:
    private:
        CryptoPP::AutoSeededRandomPool rnd_;
        CryptoPP::SecByteBlock secret_;
        CryptoPP::SecByteBlock blinded_secret_;
        CryptoPP::SecByteBlock group_secret_;
        CryptoPP::ECDH<CryptoPP::ECP>::Domain diffie_hellman_;
};

#endif /* VSOMEIP_DH_ECC_HPP */