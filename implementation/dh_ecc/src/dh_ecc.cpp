#include "../include/dh_ecc.hpp"
#include <cryptopp/nbtheory.h>
#include <cryptopp/modes.h>
#include <cryptopp/oids.h>
#include <cryptopp/asn.h>

dh_ecc::dh_ecc() {
    // Generate handshake secrets
    secret_.New(diffie_hellman_.PrivateKeyLength());
    blinded_secret_.New(diffie_hellman_.PublicKeyLength());
    diffie_hellman_.GeneratePrivateKey(rnd_, secret_);
    diffie_hellman_.GeneratePublicKey(rnd_, secret_, blinded_secret_);
    // Generate group secret
    group_secret_.New(diffie_hellman_.AgreedValueLength());
    diffie_hellman_.GeneratePrivateKey(rnd_, group_secret_);
}

dh_ecc::~dh_ecc() {
    
}

encrypted_group_secret_result dh_ecc::compute_encrypted_group_secret(CryptoPP::SecByteBlock _blinded_subscriber_secret) {
    // Compute shared secret
    CryptoPP::SecByteBlock shared_secret(diffie_hellman_.AgreedValueLength());
    diffie_hellman_.Agree(shared_secret, secret_, _blinded_subscriber_secret);
    // Calculate a SHA-256 hash over the Diffie-Hellman session key
    CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);
    CryptoPP::SHA256().CalculateDigest(key, shared_secret, shared_secret.SizeInBytes());
    // Generate a random IV
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    rnd_.GenerateBlock(iv, CryptoPP::AES::BLOCKSIZE);
    std::vector<CryptoPP::byte> iv_vector(iv, iv + CryptoPP::AES::BLOCKSIZE);
    // Encrypt
    CryptoPP::SecByteBlock encrypted_group_key(group_secret_.SizeInBytes());
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(key, CryptoPP::SHA256::DIGESTSIZE, iv);
    cfbEncryption.ProcessData(encrypted_group_key.BytePtr(), group_secret_.BytePtr(), group_secret_.SizeInBytes());
    // Fill out encrypted_group_secret_result
    encrypted_group_secret_result encrypted_groupsecret_result;
    encrypted_groupsecret_result.blinded_publisher_secret_ = blinded_secret_;
    encrypted_groupsecret_result.encrypted_group_secret_ = encrypted_group_key;
    encrypted_groupsecret_result.initialization_vector_ = iv_vector;
    return encrypted_groupsecret_result;
}

CryptoPP::SecByteBlock dh_ecc::decrypt_group_secret(encrypted_group_secret_result _encrypted_group_secret_result) {
    // Compute shared secret
    CryptoPP::SecByteBlock shared_secret(diffie_hellman_.AgreedValueLength());
    diffie_hellman_.Agree(shared_secret, secret_, _encrypted_group_secret_result.blinded_publisher_secret_);
    // Calculate a SHA-256 hash over the Diffie-Hellman session key
    CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);
    CryptoPP::SHA256().CalculateDigest(key, shared_secret, shared_secret.SizeInBytes());
    // Decrypt
    CryptoPP::SecByteBlock decrypted_group_key(diffie_hellman_.AgreedValueLength());
    CryptoPP::SecByteBlock encrypted_group_secret_ = _encrypted_group_secret_result.encrypted_group_secret_;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(key, CryptoPP::SHA256::DIGESTSIZE, _encrypted_group_secret_result.initialization_vector_.data());
    cfbDecryption.ProcessData(decrypted_group_key.BytePtr(), encrypted_group_secret_.BytePtr(), encrypted_group_secret_.SizeInBytes());
    return decrypted_group_key;
}