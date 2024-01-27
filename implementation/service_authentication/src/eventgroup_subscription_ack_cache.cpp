#include "../include/eventgroup_subscription_ack_cache.hpp"

namespace vsomeip_v3 {
    eventgroup_subscription_ack_cache::eventgroup_subscription_ack_cache() {
    }
    
    eventgroup_subscription_ack_cache::~eventgroup_subscription_ack_cache() {
    }

    void eventgroup_subscription_ack_cache::add_eventgroup_subscription_ack_cache_entry(service_t _service, instance_t _instance, eventgroup_t _eventgroup, major_version_t _major_version, ttl_t _ttl, uint8_t _counter, std::set<uint16_t> _clients, boost::asio::ip::address_v4 _sender_ip_address, boost::asio::ip::address_v4 _first_ip_address, uint16_t _port, std::vector<unsigned char> _nonce, std::vector<unsigned char> _blinded_secret, std::vector<unsigned char> _encrypted_group_secret, std::vector<unsigned char> _initialization_vector, std::vector<byte_t> _signature) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_key_tuple(_sender_ip_address, _service, _instance);
        eventgroup_subscription_ack_cache_entry& entry = eventgroup_subscription_ack_cache_map_[key_tuple];
        entry.service_ = _service;
        entry.instance_ = _instance;
        entry.eventgroup_ = _eventgroup;
        entry.major_version_ = _major_version;
        entry.ttl_ = _ttl;
        entry.counter_ = _counter;
        entry.clients_ = _clients;
        entry.sender_ip_address_ = _sender_ip_address;
        entry.first_ip_address_ = _first_ip_address;
        entry.port_ = _port;
        entry.nonce_ = _nonce;
        entry.blinded_secret_ = _blinded_secret;
        entry.encrypted_group_secret_ = _encrypted_group_secret;
        entry.initialization_vector_ = _initialization_vector;
        entry.signature_ = _signature;
    }

    void eventgroup_subscription_ack_cache::remove_eventgroup_subscription_ack_cache_entry(boost::asio::ip::address_v4 _sender_ip_address, vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_key_tuple(_sender_ip_address, _service, _instance);
        eventgroup_subscription_ack_cache_map_.erase(key_tuple);
    }

    eventgroup_subscription_ack_cache_entry eventgroup_subscription_ack_cache::get_eventgroup_subscription_ack_cache_entry(boost::asio::ip::address_v4 _sender_ip_address, vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_key_tuple(_sender_ip_address, _service, _instance);
        return eventgroup_subscription_ack_cache_map_[key_tuple];
    }

    std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t, vsomeip_v3::instance_t> eventgroup_subscription_ack_cache::make_key_tuple(boost::asio::ip::address_v4 _sender_ip_address, vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance) {
        return std::make_tuple(_sender_ip_address, _service, _instance);
    }
    
} /* end namespace vsomeip_v3 */
