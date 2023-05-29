#include "../include/svcb_cache.hpp"

namespace vsomeip_v3 {
    std::mutex svcb_cache::mutex_;
    svcb_cache* svcb_cache::instance;

    svcb_cache::svcb_cache() {
    }

    svcb_cache::~svcb_cache() {
    }

    svcb_cache* svcb_cache::get_instance() {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        if(instance == nullptr) {
            instance = new svcb_cache();
        }
        return instance;
    }
    
    void svcb_cache::add_svcb_cache_entry(service_t service_id, instance_t instance_id, major_version_t major_version, minor_version_t minor_version, int l4protocol, const boost::asio::ip::address_v4 ip_address, uint16_t port) {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        auto key_tuple = make_key_tuple(service_id, instance_id, major_version, minor_version);
        svcb_cache_entry entry;
        entry._service = service_id;
        entry._instance = instance_id;
        entry._major = major_version;
        entry._minor = minor_version;
        entry.l4protocol = l4protocol;
        entry.ip_address = ip_address;
        entry.port = port;
        svcb_cache_map[key_tuple] = entry;
    }
    
    void svcb_cache::remove_svcb_cache_entry(service_t service_id, instance_t instance_id, major_version_t major_version, minor_version_t minor_version) {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        auto key_tuple = make_key_tuple(service_id, instance_id, major_version, minor_version);
        svcb_cache_map.erase(key_tuple);
    }

    svcb_cache_entry svcb_cache::get_svcb_cache_entry(service_t service_id, instance_t instance_id, major_version_t major_version, minor_version_t minor_version) {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        auto key_tuple = make_key_tuple(service_id, instance_id, major_version, minor_version);
        return svcb_cache_map[key_tuple];
    }

    std::tuple<service_t, instance_t, major_version_t, minor_version_t> svcb_cache::make_key_tuple(service_t service_id, instance_t instance_id, major_version_t major_version, minor_version_t minor_version) {
        return std::make_tuple(service_id, instance_id, major_version, minor_version);
    }
} /* end namespace vsomeip_v3*/