#include "../include/resume_process_offerservice_cache.hpp"

namespace vsomeip_v3 {
    std::mutex resume_process_offerservice_cache::mutex_;
    resume_process_offerservice_cache* resume_process_offerservice_cache::instance;

    resume_process_offerservice_cache::resume_process_offerservice_cache() {
    }
    
    resume_process_offerservice_cache::~resume_process_offerservice_cache() {
    }

    resume_process_offerservice_cache* resume_process_offerservice_cache::getInstance() {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if(instance == nullptr) {
            instance = new resume_process_offerservice_cache();
        }
        return instance;
    }

    void resume_process_offerservice_cache::add_offerservice_entry(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor, ttl_t _ttl, const boost::asio::ip::address_v4 _reliable_address, uint16_t _reliable_port, const boost::asio::ip::address_v4 _unreliable_address, uint16_t _unreliable_port, std::vector<std::shared_ptr<sd::message_impl>> _resubscribes, bool _received_via_mcast) {
        auto key_tuple = make_key_tuple(_service, _instance, _major, _minor);
        resume_process_offerservice_map[key_tuple]._service = _service;
        resume_process_offerservice_map[key_tuple]._instance = _instance;
        resume_process_offerservice_map[key_tuple]._major = _major;
        resume_process_offerservice_map[key_tuple]._minor = _minor;
        resume_process_offerservice_map[key_tuple]._ttl = _ttl;
        resume_process_offerservice_map[key_tuple]._reliable_address = _reliable_address;
        resume_process_offerservice_map[key_tuple]._reliable_port = _reliable_port;
        resume_process_offerservice_map[key_tuple]._unreliable_address = _unreliable_address;
        resume_process_offerservice_map[key_tuple]._unreliable_port = _unreliable_port;
        resume_process_offerservice_map[key_tuple]._resubscribes = _resubscribes;
        resume_process_offerservice_map[key_tuple]._received_via_mcast = _received_via_mcast;
    }

    void resume_process_offerservice_cache::remove_offerservice_entry(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor) {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        auto key_tuple = make_key_tuple(_service, _instance, _major, _minor);
        resume_process_offerservice_map.erase(key_tuple);
    }

    resume_process_offerservice_entry resume_process_offerservice_cache::get_offerservice_entry(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor) {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        auto key_tuple = make_key_tuple(_service, _instance, _major, _minor);
        return resume_process_offerservice_map[key_tuple];
    }

    std::tuple<service_t, instance_t, major_version_t, minor_version_t> resume_process_offerservice_cache::make_key_tuple(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor) {
        return std::make_tuple(_service, _instance, _major, _minor);
    }
} /* end namespace vsomeip_v3 */