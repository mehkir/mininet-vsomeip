//
// Created by mehkir on 03.08.22.
//

#ifndef VSOMEIP_V3_RECORD_CHECKER_H
#define VSOMEIP_V3_RECORD_CHECKER_H

#define DNS_SERVER_IP 0xAC110005
#define INSTANCE 65280
#define MAJOR_VERSION 65281
#define MINOR_VERSION 65282
#define L4PROTOCOL 65283

#define ATTRLEAFBRANCH "_someip."
#define PARENTDOMAIN "service."

#include "dns_resolver.hpp"
#include "../../dnssec/include/parse_svcb_reply.hpp"
#include "../../dnssec/include/logger.hpp"
#include <vector>
#include <functional>
#include <atomic>
#include <cstring>
#include <boost/asio/ip/address_v4.hpp>
#include "vsomeip/primitive_types.hpp"

namespace vsomeip_v3 {
    typedef std::function<void(client_t, service_t, instance_t, eventgroup_t, major_version_t, event_t)> LocalCallback;
    struct LocalServiceData {
        std::atomic<client_t> client;
        service_t service;
        instance_t instance;
        eventgroup_t eventGroup;
        major_version_t major;
        event_t event;
        LocalCallback callback;
    };

    //struct remote_service_data;
    typedef std::function<void(service_t, instance_t, major_version_t,
                               minor_version_t, ttl_t, std::string,
                               uint16_t, std::string, uint16_t)> mimic_offer_callback;
    typedef std::function<void(boost::asio::ip::address_v4, service_t, instance_t, std::vector<unsigned char>)> remote_request_cache_callback;
    typedef std::function<void(void*, std::string)> remote_request_tlsa_record_callback;
    struct remote_service_data {
        service_t service;
        instance_t instance;
        major_version_t major;
        minor_version_t minor;
        boost::asio::ip::address_v4 local_ip_address;
        boost::asio::ip::address_v4 remote_ip_address;
        mimic_offer_callback offer_callback;
        remote_request_cache_callback request_cache_callback;
        remote_request_tlsa_record_callback request_tlsa_record_callback;
    };

    class record_checker {
      public:
        record_checker();
        // Dummy method always returning true
        bool is_svcb_valid();
        // Dummy method always returning true
        bool is_tlsa_valid();
        void resolveLocalSomeipService(LocalServiceData* service_data);
        void request_svcb_record(remote_service_data* service_data);
        void request_tlsa_record(void* service_data, std::string request);
      protected:
      private:
        dns_resolver* dnsResolver;
    };

} // namespace vsomeip_v3

#endif //VSOMEIP_RECORD_CHECKER_H
