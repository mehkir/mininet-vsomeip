//
// Created by mehkir on 03.08.22.
//

#ifndef VSOMEIP_V3_RECORD_CHECKER_H
#define VSOMEIP_V3_RECORD_CHECKER_H

#define DNS_SERVER_IP 0xAC110002
#define INSTANCE 65280
#define MAJOR_VERSION 65281
#define MINOR_VERSION 65282

#include "dns_resolver.hpp"
#include "../../dnssec/include/parse_svcb_reply.hpp"
#include "../../dnssec/include/logger.hpp"
#include <vector>
#include <functional>
#include <atomic>
#include <cstring>
#include <vsomeip/primitive_types.hpp>

namespace vsomeip_v3 {
    typedef std::function<void(client_t, service_t, instance_t, eventgroup_t, major_version_t, event_t)> Callback;
    struct ServiceData {
        std::atomic<client_t> client;
        service_t service;
        instance_t instance;
        eventgroup_t eventGroup;
        major_version_t major;
        event_t event;
        Callback callback;
    };

    typedef unsigned char byte;
    struct SearchResult {
        // Whether the callback has been invoked.
        bool done_ = false;
        // Explicitly provided result information.
        int status_ = ARES_ENODATA;
        int timeouts_ = 0;
        std::vector<byte> data_;
    };


    class record_checker {
      public:
        record_checker();
        // Dummy method always returning true
        bool is_svcb_valid();
        // Dummy method always returning true
        bool is_tlsa_valid();
        void resolveSomeipService(ServiceData* serviceData);
      protected:
      private:
        dns_resolver* dnsResolver;
    };

} // namespace vsomeip_v3

#endif //VSOMEIP_RECORD_CHECKER_H
