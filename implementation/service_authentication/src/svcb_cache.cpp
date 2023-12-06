#include "../include/svcb_cache.hpp"

namespace vsomeip_v3 {
    std::mutex svcb_cache::mutex_;
    svcb_cache* svcb_cache::instance_;

    svcb_cache::svcb_cache() {
    }

    svcb_cache::~svcb_cache() {
    }

    svcb_cache* svcb_cache::get_instance() {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        if(instance_ == nullptr) {
            instance_ = new svcb_cache();
        }
        return instance_;
    }
    
    void svcb_cache::add_service_svcb_cache_entry(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version, int _l4protocol, const boost::asio::ip::address_v4 _ipv4_address, uint16_t _port) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_service_key_tuple(_service, _instance, _major_version, _minor_version);
        service_svcb_cache_entry& entry = service_svcb_cache_map_[key_tuple];
        entry.service_ = _service;
        entry.instance_ = _instance;
        entry.major_ = _major_version;
        entry.minor_ = _minor_version;
        entry.l4protocol_ = _l4protocol;
        entry.ipv4_address_ = _ipv4_address;
        entry.port_ = _port;        
    }

    void svcb_cache::add_client_svcb_cache_entry(client_t _client, service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version, int _l4protocol, const boost::asio::ip::address_v4 _ipv4_address, uint16_t _port) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_client_key_tuple(_client, _service, _instance, _major_version, _minor_version);
        client_svcb_cache_entry& entry = client_svcb_cache_map_[key_tuple];
        entry.client_ = _client;
        entry.service_ = _service;
        entry.instance_ = _instance;
        entry.major_ = _major_version;
        entry.minor_ = _minor_version;
        entry.l4protocol_ = _l4protocol;
        entry.ipv4_address_ = _ipv4_address;
        entry.port_ = _port;        
    }
    
    void svcb_cache::remove_service_svcb_cache_entry(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_service_key_tuple(_service, _instance, _major_version, _minor_version);
        service_svcb_cache_map_.erase(key_tuple);
    }

    void svcb_cache::remove_client_svcb_cache_entry(client_t _client, service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_client_key_tuple(_client, _service, _instance, _major_version, _minor_version);
        client_svcb_cache_map_.erase(key_tuple);
    }

    service_svcb_cache_entry svcb_cache::get_service_svcb_cache_entry(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_service_key_tuple(_service, _instance, _major_version, _minor_version);
        return service_svcb_cache_map_[key_tuple];
    }

    client_svcb_cache_entry svcb_cache::get_client_svcb_cache_entry(client_t _client, service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_client_key_tuple(_client, _service, _instance, _major_version, _minor_version);
        return client_svcb_cache_map_[key_tuple];
    }

    std::tuple<service_t, instance_t, major_version_t, minor_version_t> svcb_cache::make_service_key_tuple(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version) {
        return std::make_tuple(_service, _instance, _major_version, _minor_version);
    }

    std::tuple<client_t, service_t, instance_t, major_version_t, minor_version_t> svcb_cache::make_client_key_tuple(client_t _client, service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version) {
        return std::make_tuple(_client, _service, _instance, _major_version, _minor_version);
    }
} /* end namespace vsomeip_v3*/