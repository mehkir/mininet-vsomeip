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
    struct service_svcb_cache_entry {
    public:
        service_t service_;
        instance_t instance_;
        major_version_t major_;
        minor_version_t minor_;
        int l4protocol_;
        boost::asio::ip::address_v4 ipv4_address_;
        uint16_t port_;
        service_svcb_cache_entry() : service_(-1), instance_(-1), major_(-1), minor_(-1), l4protocol_(-1), ipv4_address_(boost::asio::ip::address_v4::from_string("0.0.0.0")), port_(-1) {}
    };

    struct client_svcb_cache_entry : public service_svcb_cache_entry {
        client_t client_;
        client_svcb_cache_entry() : client_(-1) {}
    };


    class svcb_cache {
        private:
            svcb_cache();
            ~svcb_cache();
            static std::mutex mutex_;
            static svcb_cache* instance_;
            std::map<std::tuple<service_t, instance_t, major_version_t, minor_version_t>, service_svcb_cache_entry> service_svcb_cache_map_;
            std::map<std::tuple<client_t, service_t, instance_t, major_version_t>, client_svcb_cache_entry> client_svcb_cache_map_;
        public:
            static svcb_cache* get_instance();

            svcb_cache(svcb_cache const&) = delete;
            svcb_cache(svcb_cache&&) = delete;
            svcb_cache& operator=(svcb_cache &) = delete;
            svcb_cache& operator=(svcb_cache &&) = delete;

            void add_service_svcb_cache_entry(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version, int _l4protocol, const boost::asio::ip::address_v4 _ipv4_address, uint16_t _port);
            void add_client_svcb_cache_entry(client_t _client, service_t _service, instance_t _instance, major_version_t _major_version, int _l4protocol, const boost::asio::ip::address_v4 _ipv4_address, uint16_t _port);
            void remove_service_svcb_cache_entry(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version);
            void remove_client_svcb_cache_entry(client_t _client, service_t _service, instance_t _instance, major_version_t _major_version);
            service_svcb_cache_entry get_service_svcb_cache_entry(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version);
            client_svcb_cache_entry get_client_svcb_cache_entry(client_t _client, service_t _service, instance_t _instance, major_version_t _major_version);
            std::tuple<service_t, instance_t, major_version_t, minor_version_t> make_service_key_tuple(service_t _service, instance_t _instance, major_version_t _major_version, minor_version_t _minor_version);
            std::tuple<client_t, service_t, instance_t, major_version_t> make_client_key_tuple(client_t _client, service_t _service, instance_t _instance, major_version_t _major_version);
    };

} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_SVCB_CACHE_HPP */