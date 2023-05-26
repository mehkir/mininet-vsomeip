#ifndef VSOMEIP_V3_EVENTGROUP_SUBSCRPTION_ACK_CACHE_HPP
#define VSOMEIP_V3_EVENTGROUP_SUBSCRPTION_ACK_CACHE_HPP

#include <mutex>
#include <map>
#include <tuple>
#include <set>
#include <boost/asio/ip/address_v4.hpp>
#include "vsomeip/primitive_types.hpp"

namespace vsomeip_v3 {
    struct eventgroup_subscription_ack_cache_entry {
        service_t _service = -1;
        instance_t _instance = -1;
        eventgroup_t _eventgroup = -1;
        major_version_t _major_version = -1;
        ttl_t _ttl = -1;
        uint8_t _counter = -1;
        std::set<uint16_t> _clients = std::set<uint16_t>();
        boost::asio::ip::address_v4 _sender_ip_address = boost::asio::ip::address_v4::from_string("0.0.0.0");
        boost::asio::ip::address_v4 _first_ip_address = boost::asio::ip::address_v4::from_string("0.0.0.0");
        uint16_t _port = -1;
    };

    class eventgroup_subscription_ack_cache {
    private:
        eventgroup_subscription_ack_cache();
        ~eventgroup_subscription_ack_cache();
        static std::mutex mutex_;
        static eventgroup_subscription_ack_cache* instance;
        std::map<std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t, vsomeip_v3::instance_t>, eventgroup_subscription_ack_cache_entry> eventgroup_subscription_ack_cache_map;
    public:
        static eventgroup_subscription_ack_cache* get_instance();

        eventgroup_subscription_ack_cache(eventgroup_subscription_ack_cache const&) = delete;
        eventgroup_subscription_ack_cache(eventgroup_subscription_ack_cache&&) = delete;
        eventgroup_subscription_ack_cache& operator=(eventgroup_subscription_ack_cache &) = delete;
        eventgroup_subscription_ack_cache& operator=(eventgroup_subscription_ack_cache &&) = delete;

        void add_eventgroup_subscription_ack_cache_entry(service_t _service, instance_t _instance, eventgroup_t _eventgroup, major_version_t _major_version, ttl_t _ttl, uint8_t _counter, std::set<uint16_t> _clients, boost::asio::ip::address_v4 _sender_ip_address, boost::asio::ip::address_v4 _first_ip_address, uint16_t _port);
        void remove_eventgroup_subscription_ack_cache_entry(boost::asio::ip::address_v4 _sender_ip_address, vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance);
        eventgroup_subscription_ack_cache_entry get_eventgroup_subscription_ack_cache_entry(boost::asio::ip::address_v4 _sender_ip_address, vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance);
        std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t, vsomeip_v3::instance_t> make_key_tuple(boost::asio::ip::address_v4 _sender_ip_address, vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance);
    };
    
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_EVENTGROUP_SUBSCRPTION_ACK_CACHE_HPP */