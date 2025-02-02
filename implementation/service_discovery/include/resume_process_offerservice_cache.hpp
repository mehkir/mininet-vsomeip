#ifndef VSOMEIP_V3_RESUME_PROCESS_OFFERSERVICE_CACHE_H
#define VSOMEIP_V3_RESUME_PROCESS_OFFERSERVICE_CACHE_H

#include <mutex>
#include <vsomeip/primitive_types.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <vector>
#include <map>
#include <tuple>
#include "message_impl.hpp"

namespace vsomeip_v3 {
    struct resume_process_offerservice_entry {
        service_t service_ = -1;
        instance_t instance_ = -1;
        major_version_t major_ = -1;
        minor_version_t minor_ = -1;
        ttl_t ttl_ = -1;
        boost::asio::ip::address_v4 reliable_address_ = boost::asio::ip::address_v4::from_string("0.0.0.0");
        uint16_t reliable_port_ = -1;
        boost::asio::ip::address_v4 unreliable_address_ = boost::asio::ip::address_v4::from_string("0.0.0.0");
        uint16_t unreliable_port_ = -1;
        std::vector<std::shared_ptr<sd::message_impl> > resubscribes_;
        bool received_via_mcast_ = false;
    };

    class resume_process_offerservice_cache {
    private:
        resume_process_offerservice_cache();
        ~resume_process_offerservice_cache();
        static std::mutex mutex_;
        static resume_process_offerservice_cache* instance_;
        std::map<std::tuple<service_t, instance_t, major_version_t, minor_version_t>, resume_process_offerservice_entry> resume_process_offerservice_map_;
        std::tuple<service_t, instance_t, major_version_t, minor_version_t> make_key_tuple(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor);
    public:
        static resume_process_offerservice_cache* get_instance();

        resume_process_offerservice_cache(resume_process_offerservice_cache const&) = delete;
        resume_process_offerservice_cache(resume_process_offerservice_cache&&) = delete;
        resume_process_offerservice_cache& operator=(resume_process_offerservice_cache &) = delete;
        resume_process_offerservice_cache& operator=(resume_process_offerservice_cache &&) = delete;

        void add_offerservice_entry(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor, ttl_t _ttl, const boost::asio::ip::address_v4 _reliable_address, uint16_t _reliable_port, const boost::asio::ip::address_v4 _unreliable_address, uint16_t _unreliable_port, std::vector<std::shared_ptr<sd::message_impl>> _resubscribes, bool _received_via_mcast);
        void remove_offerservice_entry(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor);
        resume_process_offerservice_entry get_offerservice_entry(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor);
    };
    
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_RESUME_PROCESS_OFFERSERVICE_CACHE_H */