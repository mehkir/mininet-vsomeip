#include "../include/eventgroup_subscription_ack_cache.hpp"

namespace vsomeip_v3 {
    std::mutex eventgroup_subscription_ack_cache::mutex_;
    eventgroup_subscription_ack_cache* eventgroup_subscription_ack_cache::instance;

    eventgroup_subscription_ack_cache::eventgroup_subscription_ack_cache() {
    }
    
    eventgroup_subscription_ack_cache::~eventgroup_subscription_ack_cache() {
    }

    eventgroup_subscription_ack_cache* eventgroup_subscription_ack_cache::get_instance() {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if(instance == nullptr) {
            instance = new eventgroup_subscription_ack_cache();
        }
        return instance;
    }

    void eventgroup_subscription_ack_cache::add_eventgroup_subscription_ack_cache_entry(service_t _service, instance_t _instance, eventgroup_t _eventgroup, major_version_t _major_version, ttl_t _ttl, uint8_t _counter, std::set<uint16_t> _clients, boost::asio::ip::address_v4 _sender_ip_address, boost::asio::ip::address_v4 _first_ip_address, uint16_t _port, std::vector<unsigned char> _nonce, std::vector<byte_t> _signature) {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        auto key_tuple = make_key_tuple(_sender_ip_address, _service, _instance);
        eventgroup_subscription_ack_cache_entry& entry = eventgroup_subscription_ack_cache_map[key_tuple];
        entry._service = _service;
        entry._instance = _instance;
        entry._eventgroup = _eventgroup;
        entry._major_version = _major_version;
        entry._ttl = _ttl;
        entry._counter = _counter;
        entry._clients = _clients;
        entry._sender_ip_address = _sender_ip_address;
        entry._first_ip_address = _first_ip_address;
        entry._port = _port;
        entry._nonce = _nonce;
        entry._signature = _signature;
    }

    void eventgroup_subscription_ack_cache::remove_eventgroup_subscription_ack_cache_entry(boost::asio::ip::address_v4 _sender_ip_address, vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance) {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        auto key_tuple = make_key_tuple(_sender_ip_address, _service, _instance);
        eventgroup_subscription_ack_cache_map.erase(key_tuple);
    }

    eventgroup_subscription_ack_cache_entry eventgroup_subscription_ack_cache::get_eventgroup_subscription_ack_cache_entry(boost::asio::ip::address_v4 _sender_ip_address, vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance) {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        auto key_tuple = make_key_tuple(_sender_ip_address, _service, _instance);
        return eventgroup_subscription_ack_cache_map[key_tuple];
    }

    std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t, vsomeip_v3::instance_t> eventgroup_subscription_ack_cache::make_key_tuple(boost::asio::ip::address_v4 _sender_ip_address, vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance) {
        return std::make_tuple(_sender_ip_address, _service, _instance);
    }
    
} /* end namespace vsomeip_v3 */
