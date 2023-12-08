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
            void request_service_svcb_record(service_data_and_cbs* _service_data_and_cbs);
            void request_client_svcb_record(client_data_and_cbs* _client_data_and_cbs);
    };
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_SVCB_RESOLVER_H */
