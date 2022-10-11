//
// Created by mehkir on 03.08.22.
//

#include "../include/record_checker.hpp"
#include "../include/logger.hpp"
#include "../include/parse_svcb_reply.hpp"
#include <arpa/nameser.h>
#include <string.h>

namespace vsomeip_v3 {

    record_checker::record_checker() : dnsResolver(dns_resolver::getInstance()) {
        dnsResolver->initialize(DNS_SERVER_IP);
    }

    bool record_checker::is_svcb_valid() {
        return true;
    }

    void record_checker::resolveSomeipService(ServiceData* serviceData) {
        dnsResolver->resolve("_someip._udp."<< serviceData->service <<".service.", C_IN, T_SVCB, ResolverCallback, serviceData);
    }

    bool record_checker::is_tlsa_valid() {
        return true;
    }

} // namespace vsomeip_v3