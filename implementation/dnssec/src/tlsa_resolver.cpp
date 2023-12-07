#include "../include/tlsa_resolver.hpp"
#include "../../timestamps/include/timestamp_collector.hpp"
#include "../include/parse_tlsa_reply.hpp"
#include <arpa/nameser.h>
#include <netinet/in.h>
#include <cstring>

namespace vsomeip_v3 {
    tlsa_resolver::tlsa_resolver() : dns_resolver_(dns_resolver::get_instance()) {
        dns_resolver_->initialize(DNS_SERVER_IP);
    }

    tlsa_resolver::~tlsa_resolver() {
    }

    void tlsa_resolve_callback(void* _data, int _status, int _timeouts,
                unsigned char* _abuf, int _alen) {
        service_data_and_cbs* servicedata_and_cbs = reinterpret_cast<service_data_and_cbs*>(_data);
        
        if (_status) {
            std::cerr << __func__ << " Bad DNS response" << std::endl;
            return;
        }

        if (_timeouts) {
            std::cerr << __func__ << " DNS request timeout" << std::endl;
            return;
        }

        unsigned char* copy = new unsigned char[_alen];
        memcpy(copy, _abuf, _alen);
        tlsa_reply* tlsareply;
        if ((parse_tlsa_reply(copy, _alen, &tlsareply)) != ARES_SUCCESS) {
            std::cerr << "Parsing TLSA reply failed" << std::endl;
            delete_tlsa_reply(tlsareply);
            return;
        }

        tlsa_reply* tlsa_reply_ptr = tlsareply;
        while (tlsa_reply_ptr != nullptr) {
            servicedata_and_cbs->add_publisher_certificate_callback_(servicedata_and_cbs->ipv4_address_, servicedata_and_cbs->service_, servicedata_and_cbs->instance_, servicedata_and_cbs->convert_der_to_pem_callback_(tlsa_reply_ptr->certificate_association_data_));
            servicedata_and_cbs->verify_publisher_signature_callback_(servicedata_and_cbs->ipv4_address_, servicedata_and_cbs->service_, servicedata_and_cbs->instance_);
            tlsa_reply_ptr = tlsa_reply_ptr->tlsa_reply_next_;
        }
        delete_tlsa_reply(tlsareply);
        
        delete servicedata_and_cbs;
        delete[] copy;
    }

    void tlsa_resolver::request_service_tlsa_record(void* _service_data, std::string _tlsa_request) {
        dns_resolver_->resolve(_tlsa_request.c_str(), C_IN, T_TLSA, tlsa_resolve_callback, _service_data);
    }
} /* end namespace vsomeip_v3 */