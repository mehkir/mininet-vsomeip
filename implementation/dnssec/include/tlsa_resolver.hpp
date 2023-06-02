#ifndef VSOMEIP_V3_TLSA_RESOLVER_H
#define VSOMEIP_V3_TLSA_RESOLVER_H

#include "dns_resolver.hpp"
#include "../include/someip_dns_parameters.hpp"

namespace vsomeip_v3 {
    class tlsa_resolver {
        private:
            dns_resolver* dns_resolver_;
        public:
            tlsa_resolver();
            ~tlsa_resolver();
            void request_tlsa_record(void* _service_data, std::string _request);
    };
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_TLSA_RESOLVER_H */