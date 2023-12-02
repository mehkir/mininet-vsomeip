#include "../include/request_cache.hpp"
#include <iostream>

namespace vsomeip_v3 {
    std::mutex request_cache::mutex_;
    request_cache* request_cache::instance_;

    request_cache::request_cache() {
    }

    request_cache::~request_cache() {
    }

    request_cache* request_cache::get_instance() {
        std::lock_guard<std::mutex> guard(mutex_);
        if(instance_ == nullptr) {
            instance_ = new request_cache();
        }
        return instance_;
    }

    void request_cache::add_request_nonce(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                                vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce) {
        std::lock_guard<std::mutex> guard(mutex_);
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        request_map_[key_tuple].nonce_ = _nonce;
    }

    bool request_cache::has_nonce_and_remove(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce) {
        std::lock_guard<std::mutex> guard(mutex_);
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        bool has_nonce = request_map_.count(key_tuple) && !request_map_[key_tuple].nonce_.empty() && request_map_[key_tuple].nonce_ == _nonce;
        if (has_nonce) { request_map_[key_tuple].nonce_.clear(); }
        return has_nonce;
    }

    void request_cache::add_request_certificate(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance, std::vector<unsigned char> _certificate_data) {
        std::lock_guard<std::mutex> guard(mutex_);
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        request_map_[key_tuple].certificate_data_ = _certificate_data;
    }

    std::vector<unsigned char> request_cache::get_request_nonce(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance) {
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        return request_map_[key_tuple].nonce_;
    }

    std::vector<unsigned char> request_cache::get_request_certificate(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance) {
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        return request_map_[key_tuple].certificate_data_;
    }

    void request_cache::remove_request(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance) {
        auto key_tuple = make_key_tupe(_ipv4_address, _service, _instance);
        request_map_.erase(key_tuple);
    }

    std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t,
                        vsomeip_v3::instance_t> request_cache::make_key_tupe(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                        vsomeip_v3::instance_t _instance) {
        return std::make_tuple(_ipv4_address, _service, _instance);
    }
} /* end namespace vsomeip_v3 */