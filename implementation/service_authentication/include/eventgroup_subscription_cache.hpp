#ifndef VSOMEIP_V3_EVENTGROUP_SUBSCRIPTION_CACHE_HPP
#define VSOMEIP_V3_EVENTGROUP_SUBSCRIPTION_CACHE_HPP

#include <mutex>
#include <vsomeip/primitive_types.hpp>
#include "../../routing/include/eventgroupinfo.hpp"
#include "../../service_discovery/include/remote_subscription_ack.hpp"
#include <boost/asio/ip/address_v4.hpp>
#include <vector>
#include <set>
#include <map>
#include <tuple>

namespace vsomeip_v3 {
    struct eventgroup_subscription_cache_entry {
        public:
            client_t client_ = -1;
            service_t service_ = -1;
            instance_t instance_ = -1;
            eventgroup_t eventgroup_ = -1;
            major_version_t major_ = -1;
            ttl_t ttl_ = -1;
            uint8_t counter_ = -1;
            uint16_t reserved_ = -1;
            boost::asio::ip::address_v4 first_address_ = boost::asio::ip::address_v4::from_string("0.0.0.0");
            uint16_t first_port_ = -1;
            bool is_first_reliable_ = false;
            boost::asio::ip::address_v4 second_address_ = boost::asio::ip::address_v4::from_string("0.0.0.0");
            uint16_t second_port_ = -1;
            bool is_second_reliable_ = false;
            std::shared_ptr<sd::remote_subscription_ack> acknowledgement_;
            bool is_stop_subscribe_subscribe_ = false;
            bool force_initial_events_ = false;
            std::set<client_t> clients_;
            std::set<std::pair<bool, std::uint16_t>> expired_ports_;
            bool sd_acceptance_required_;
            bool accept_entries_;         
            std::shared_ptr<eventgroupinfo> info_;
            std::vector<unsigned char> nonce_;
            std::vector<unsigned char> blinded_secret_;
            std::vector<byte_t> signature_;
    };

    class eventgroup_subscription_cache {
    private:
        std::mutex mutex_;
        std::map<std::tuple<client_t, service_t, instance_t, major_version_t>, eventgroup_subscription_cache_entry> eventgroup_subscription_cache_map_;
        std::tuple<client_t, service_t, instance_t, major_version_t> make_key_tuple(client_t _client, service_t _service, instance_t _instance, major_version_t _major);
    public:
        eventgroup_subscription_cache();
        ~eventgroup_subscription_cache();

        eventgroup_subscription_cache(eventgroup_subscription_cache const&) = delete;
        eventgroup_subscription_cache(eventgroup_subscription_cache&&) = delete;
        eventgroup_subscription_cache& operator=(eventgroup_subscription_cache &) = delete;
        eventgroup_subscription_cache& operator=(eventgroup_subscription_cache &&) = delete;

        void add_eventgroup_subscription_cache_entry(client_t _client, service_t _service, instance_t _instance, eventgroup_t _eventgroup, major_version_t _major, ttl_t _ttl, uint8_t _counter, uint16_t _reserved, const boost::asio::ip::address_v4 _first_address, uint16_t _first_port, bool _is_first_reliable,
                                const boost::asio::ip::address_v4 _second_address, uint16_t _second_port, bool _is_second_reliable, std::shared_ptr<sd::remote_subscription_ack> _acknowledgement, bool _is_stop_subscribe_subscribe, bool _force_initial_events, std::set<client_t> _clients,
                                std::set<std::pair<bool, std::uint16_t>> _expired_ports, bool _sd_acceptance_required, bool _accept_entries, std::shared_ptr<eventgroupinfo> _info, std::vector<unsigned char> _nonce, std::vector<unsigned char> _blinded_secret, std::vector<byte_t> _signature);
        void remove_eventgroup_subscription_cache_entry(client_t _client, service_t _service, instance_t _instance, major_version_t _major);
        eventgroup_subscription_cache_entry get_eventgroup_subscription_cache_entry(client_t client, service_t _service, instance_t _instance, major_version_t _major);
    };
    
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_EVENTGROUP_SUBSCRIPTION_CACHE_HPP */