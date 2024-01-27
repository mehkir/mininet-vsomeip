#include "../include/eventgroup_subscription_cache.hpp"

namespace vsomeip_v3 {
    eventgroup_subscription_cache::eventgroup_subscription_cache() {
    }
    
    eventgroup_subscription_cache::~eventgroup_subscription_cache() {
    }

    void eventgroup_subscription_cache::add_eventgroup_subscription_cache_entry(client_t _client, service_t _service, instance_t _instance, eventgroup_t _eventgroup, major_version_t _major, ttl_t _ttl, uint8_t _counter, uint16_t _reserved, const boost::asio::ip::address_v4 _first_address, uint16_t _first_port, bool _is_first_reliable,
                                const boost::asio::ip::address_v4 _second_address, uint16_t _second_port, bool _is_second_reliable, std::shared_ptr<sd::remote_subscription_ack> _acknowledgement, bool _is_stop_subscribe_subscribe, bool _force_initial_events, std::set<client_t> _clients,
                                std::set<std::pair<bool, std::uint16_t>> _expired_ports, bool _sd_acceptance_required, bool _accept_entries, std::shared_ptr<eventgroupinfo> _info, std::vector<unsigned char> _nonce, std::vector<unsigned char> _blinded_secret, std::vector<byte_t> _signature) {
        auto key_tuple = make_key_tuple(_client, _service, _instance, _major);
        eventgroup_subscription_cache_entry& entry = eventgroup_subscription_cache_map_[key_tuple];
        entry.client_ = _client;
        entry.service_ = _service;
        entry.instance_ = _instance;
        entry.eventgroup_ = _eventgroup;
        entry.major_ = _major;
        entry.ttl_ = _ttl;
        entry.counter_ = _counter;
        entry.reserved_ = _reserved;
        entry.first_address_ = _first_address;
        entry.first_port_ = _first_port;
        entry.is_first_reliable_ = _is_first_reliable;
        entry.second_address_ = _second_address;
        entry.second_port_ = _second_port;
        entry.is_second_reliable_ = _is_second_reliable;
        entry.acknowledgement_ = _acknowledgement;
        entry.is_stop_subscribe_subscribe_ = _is_stop_subscribe_subscribe;
        entry.force_initial_events_ = _force_initial_events;
        entry.clients_ = _clients;
        entry.expired_ports_ = _expired_ports;
        entry.sd_acceptance_required_ = _sd_acceptance_required;
        entry.accept_entries_ = _accept_entries;
        entry.info_ = _info;
        entry.nonce_ = _nonce;
        entry.blinded_secret_ = _blinded_secret;
        entry.signature_ = _signature;
    }

    void eventgroup_subscription_cache::remove_eventgroup_subscription_cache_entry(client_t _client, service_t _service, instance_t _instance, major_version_t _major) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_key_tuple(_client, _service, _instance, _major);
        eventgroup_subscription_cache_map_.erase(key_tuple);
    }

    eventgroup_subscription_cache_entry eventgroup_subscription_cache::get_eventgroup_subscription_cache_entry(client_t _client, service_t _service, instance_t _instance, major_version_t _major) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_key_tuple(_client, _service, _instance, _major);
        return eventgroup_subscription_cache_map_[key_tuple];
    }

    std::tuple<client_t, service_t, instance_t, major_version_t> eventgroup_subscription_cache::make_key_tuple(client_t _client, service_t _service, instance_t _instance, major_version_t _major) {
        return std::make_tuple(_client, _service, _instance, _major);
    }
} /* end namespace vsomeip_v3 */