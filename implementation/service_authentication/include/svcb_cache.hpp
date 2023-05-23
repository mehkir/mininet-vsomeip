#ifndef VSOMEIP_V3_SVCB_CACHE_HPP
#define VSOMEIP_V3_SVCB_CACHE_HPP

#include <mutex>
#include <map>
#include <tuple>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include "vsomeip/primitive_types.hpp"

namespace vsomeip_v3 {
    struct svcb_cache_entry {
        service_t _service = -1;
        instance_t _instance = -1;
        major_version_t _major = -1;
        minor_version_t _minor = -1;
        int l4protocol = -1;
        boost::asio::ip::address_v4 ip_address = boost::asio::ip::address_v4::from_string("0.0.0.0");
        uint16_t port = -1;
    };

    class svcb_cache {
        private:
            svcb_cache();
            ~svcb_cache();
            static std::mutex mutex_;
            static svcb_cache* instance;
            std::map<std::tuple<service_t, instance_t, major_version_t, minor_version_t>, svcb_cache_entry> svcb_cache_map;
        public:
            static svcb_cache* getInstance();

            svcb_cache(svcb_cache const&) = delete;
            svcb_cache(svcb_cache&&) = delete;
            svcb_cache& operator=(svcb_cache &) = delete;
            svcb_cache& operator=(svcb_cache &&) = delete;

            void add_svcb_cache_entry(service_t service_id, instance_t instance_id, major_version_t major_version, minor_version_t minor_version, int l4protocol, const boost::asio::ip::address_v4 ip_address, uint16_t port);
            void remove_svcb_cache_entry(service_t service_id, instance_t instance_id, major_version_t major_version, minor_version_t minor_version);
            svcb_cache_entry get_svcb_cache_entry(service_t service_id, instance_t instance_id, major_version_t major_version, minor_version_t minor_version);
            std::tuple<service_t, instance_t, major_version_t, minor_version_t> make_key_tuple(service_t service_id, instance_t instance_id, major_version_t major_version, minor_version_t minor_version);


    };

} /* end namespace vsomeip_v3*/
#endif /* VSOMEIP_V3_SVCB_CACHE_HPP */