#ifndef VSOMEIP_V3_SVCB_RESOLVER_H
#define VSOMEIP_V3_SVCB_RESOLVER_H

#include "dns_resolver.hpp"
#include "someip_dns_parameters.hpp"

namespace vsomeip_v3 {
    class svcb_resolver {
        private:
            dns_resolver* dns_resolver_;
        public:
            svcb_resolver();
            ~svcb_resolver();
            void request_svcb_record(service_data_and_cbs* service_data_and_cbs_);
    };
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_SVCB_RESOLVER_H */
