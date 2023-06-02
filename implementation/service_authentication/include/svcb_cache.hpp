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
        service_t service_ = -1;
        instance_t instance_ = -1;
        major_version_t major_ = -1;
        minor_version_t minor_ = -1;
        int l4protocol_ = -1;
        boost::asio::ip::address_v4 ipv4_address_ = boost::asio::ip::address_v4::from_string("0.0.0.0");
        uint16_t port_ = -1;
    };

    class svcb_cache {
        private:
            svcb_cache();
            ~svcb_cache();
            static std::mutex mutex_;
            static svcb_cache* instance_;
            std::map<std::tuple<service_t, instance_t, major_version_t, minor_version_t>, svcb_cache_entry> svcb_cache_map_;
        public:
            static svcb_cache* get_instance();

            svcb_cache(svcb_cache const&) = delete;
            svcb_cache(svcb_cache&&) = delete;
            svcb_cache& operator=(svcb_cache &) = delete;
            svcb_cache& operator=(svcb_cache &&) = delete;

            void add_svcb_cache_entry(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version, int _l4protocol, const boost::asio::ip::address_v4 _ipv4_address, uint16_t _port);
            void remove_svcb_cache_entry(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version);
            svcb_cache_entry get_svcb_cache_entry(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version);
            std::tuple<service_t, instance_t, major_version_t, minor_version_t> make_key_tuple(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version);


    };

} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_SVCB_CACHE_HPP */