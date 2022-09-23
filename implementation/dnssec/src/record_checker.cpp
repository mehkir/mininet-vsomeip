//
// Created by mehkir on 03.08.22.
//

#include "../include/record_checker.hpp"
#include "../include/dns_resolver.hpp"

namespace vsomeip_v3 {

    record_checker::record_checker() {

    }

    bool record_checker::is_svcb_valid() {

        return true;
    }

    bool record_checker::is_tlsa_valid() {

        return true;
    }
} // namespace vsomeip_v3