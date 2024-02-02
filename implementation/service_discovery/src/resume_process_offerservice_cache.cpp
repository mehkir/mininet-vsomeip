#include "../include/resume_process_offerservice_cache.hpp"

namespace vsomeip_v3 {
    std::mutex resume_process_offerservice_cache::mutex_;
    resume_process_offerservice_cache* resume_process_offerservice_cache::instance_;

    resume_process_offerservice_cache::resume_process_offerservice_cache() {
    }
    
    resume_process_offerservice_cache::~resume_process_offerservice_cache() {
    }

    resume_process_offerservice_cache* resume_process_offerservice_cache::get_instance() {
        std::lock_guard<std::mutex> lockguard(mutex_);
        if(instance_ == nullptr) {
            instance_ = new resume_process_offerservice_cache();
        }
        return instance_;
    }

    void resume_process_offerservice_cache::add_offerservice_entry(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor, ttl_t _ttl, const boost::asio::ip::address_v4 _reliable_address, uint16_t _reliable_port, const boost::asio::ip::address_v4 _unreliable_address, uint16_t _unreliable_port, std::vector<std::shared_ptr<sd::message_impl>> _resubscribes, bool _received_via_mcast) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_key_tuple(_service, _instance, _major, _minor);
        resume_process_offerservice_entry& entry = resume_process_offerservice_map_[key_tuple];
        entry.service_ = _service;
        entry.instance_ = _instance;
        entry.major_ = _major;
        entry.minor_ = _minor;
        entry.ttl_ = _ttl;
        entry.reliable_address_ = _reliable_address;
        entry.reliable_port_ = _reliable_port;
        entry.unreliable_address_ = _unreliable_address;
        entry.unreliable_port_ = _unreliable_port;
        entry.resubscribes_ = _resubscribes;
        entry.received_via_mcast_ = _received_via_mcast;
    }

    void resume_process_offerservice_cache::remove_offerservice_entry(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_key_tuple(_service, _instance, _major, _minor);
        resume_process_offerservice_map_.erase(key_tuple);
    }

    resume_process_offerservice_entry resume_process_offerservice_cache::get_offerservice_entry(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor) {
        std::lock_guard<std::mutex> lockguard(mutex_);
        auto key_tuple = make_key_tuple(_service, _instance, _major, _minor);
        return resume_process_offerservice_map_[key_tuple];
    }

    std::tuple<service_t, instance_t, major_version_t, minor_version_t> resume_process_offerservice_cache::make_key_tuple(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor) {
        return std::make_tuple(_service, _instance, _major, _minor);
    }
} /* end namespace vsomeip_v3 */