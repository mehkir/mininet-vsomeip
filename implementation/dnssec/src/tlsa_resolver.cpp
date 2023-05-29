#include "../include/tlsa_resolver.hpp"
#include "../../timestamps/include/timestamp_collector.hpp"
#include "../include/parse_tlsa_reply.hpp"
#include <arpa/nameser.h>
#include <netinet/in.h>
#include <cstring>

namespace vsomeip_v3 {
    tlsa_resolver::tlsa_resolver() : dns_resolver_(dns_resolver::getInstance()) {
        dns_resolver_->initialize(DNS_SERVER_IP);
    }

    tlsa_resolver::~tlsa_resolver() {
    }

    void tlsa_resolve_callback(void *data, int status, int timeouts,
                unsigned char *abuf, int alen) {
        service_data_and_cbs* service_data_and_cbs_ = reinterpret_cast<service_data_and_cbs*>(data);
        
        if (status) {
            std::cout << "Bad DNS response" << std::endl;
            return;
        }

        unsigned char* copy = new unsigned char[alen];
        memcpy(copy, abuf, alen);
        TLSA_Reply* tlsa_reply;
        if ((parse_tlsa_reply(copy, alen, &tlsa_reply)) != ARES_SUCCESS) {
            std::cout << "Parsing TLSA reply failed" << std::endl;
            delete_tlsa_reply(tlsa_reply);
            return;
        }

        TLSA_Reply* tlsa_reply_ptr = tlsa_reply;
        while (tlsa_reply_ptr != nullptr) {
            service_data_and_cbs_->request_cache_callback_(service_data_and_cbs_->ip_address, service_data_and_cbs_->service, service_data_and_cbs_->instance, service_data_and_cbs_->convert_DER_to_PEM_callback_(tlsa_reply_ptr->certificate_association_data));
            service_data_and_cbs_->verify_publisher_signature_callback(service_data_and_cbs_->ip_address, service_data_and_cbs_->service, service_data_and_cbs_->instance);
            tlsa_reply_ptr = tlsa_reply_ptr->tlsaReplyNext;
        }
        delete_tlsa_reply(tlsa_reply);
        
        delete service_data_and_cbs_;
        delete[] copy;
    }

    void tlsa_resolver::request_tlsa_record(void* service_data, std::string tlsa_request) {
        dns_resolver_->resolve(tlsa_request.c_str(), C_IN, T_TLSA, tlsa_resolve_callback, service_data);
    }
} /* end namespace vsomeip_v3 */