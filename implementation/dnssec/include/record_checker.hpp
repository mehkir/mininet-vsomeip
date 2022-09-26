//
// Created by mehkir on 03.08.22.
//

#ifndef VSOMEIP_V3_RECORD_CHECKER_H
#define VSOMEIP_V3_RECORD_CHECKER_H

#include "dns_resolver.hpp"
#include <vector>

namespace vsomeip_v3 {
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
      protected:
      private:
        dns_resolver* dnsResolver;
        ares_callback callback;
    };

} // namespace vsomeip_v3

#endif //VSOMEIP_RECORD_CHECKER_H
