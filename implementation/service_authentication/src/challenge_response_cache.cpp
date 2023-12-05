#include "../include/challenge_response_cache.hpp"
#include <iostream>

namespace vsomeip_v3 {
    challenge_response_cache::challenge_response_cache() {
    }

    challenge_response_cache::~challenge_response_cache() {
    }
    void challenge_response_cache::add_nonce(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                                vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce) {
        std::lock_guard<std::mutex> guard(mutex_);
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        challenge_response_map_[key_tuple].nonce_ = _nonce;
    }

    bool challenge_response_cache::has_nonce_and_remove(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce) {
        std::lock_guard<std::mutex> guard(mutex_);
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        bool has_nonce = challenge_response_map_.count(key_tuple) && !challenge_response_map_[key_tuple].nonce_.empty() && challenge_response_map_[key_tuple].nonce_ == _nonce;
        if (has_nonce) { challenge_response_map_[key_tuple].nonce_.clear(); }
        return has_nonce;
    }

    void challenge_response_cache::add_certificate(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance, std::vector<unsigned char> _certificate_data) {
        std::lock_guard<std::mutex> guard(mutex_);
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        challenge_response_map_[key_tuple].certificate_data_ = _certificate_data;
    }

    std::vector<unsigned char> challenge_response_cache::get_nonce(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance) {
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        return challenge_response_map_[key_tuple].nonce_;
    }

    std::vector<unsigned char> challenge_response_cache::get_certificate(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance) {
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        return challenge_response_map_[key_tuple].certificate_data_;
    }

    void challenge_response_cache::remove_entry(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance) {
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        challenge_response_map_.erase(key_tuple);
    }

    std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t,
                        vsomeip_v3::instance_t> challenge_response_cache::make_key_tupe(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance) {
        return std::make_tuple(_ipv4_address, _service, _instance);
    }
    
    void challenge_response_cache::set_offered_nonce(vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce) {
        std::lock_guard<std::mutex> guard(mutex_);
        auto key_tuple = make_key_tupe(_service, _instance);
        offered_nonce_map_[key_tuple] = _nonce;
    }

    std::vector<unsigned char> challenge_response_cache::get_offered_nonce(vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance) {
        std::lock_guard<std::mutex> guard(mutex_);
        auto key_tuple = make_key_tupe(_service, _instance);
        return offered_nonce_map_[key_tuple];
    }

    std::tuple<vsomeip_v3::service_t, vsomeip_v3::instance_t> challenge_response_cache::make_key_tupe(vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance) {
        return std::make_tuple(_service, _instance);
    }
} /* end namespace vsomeip_v3 */