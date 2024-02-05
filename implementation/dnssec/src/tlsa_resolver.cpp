#include "../include/tlsa_resolver.hpp"
#include "../include/parse_tlsa_reply.hpp"
#include <vsomeip/internal/logger.hpp>
#include <arpa/nameser.h>
#include <netinet/in.h>
#include <cstring>

namespace vsomeip_v3 {
    tlsa_resolver::tlsa_resolver(in_addr_t _dns_server_ip) : dns_resolver_(dns_resolver::get_instance()) {
        dns_resolver_->initialize(_dns_server_ip);
    }

    tlsa_resolver::~tlsa_resolver() {
    }

    void service_tlsa_resolve_callback(void* _data, int _status, int _timeouts,
                unsigned char* _abuf, int _alen) {
        service_data_and_cbs* servicedata_and_cbs = reinterpret_cast<service_data_and_cbs*>(_data);
        
        if (_status) {
            std::cerr << __func__ << " Bad DNS response" << std::endl;
            delete servicedata_and_cbs;
            return;
        }

        if (_timeouts) {
            std::cerr << __func__ << " DNS request timeout" << std::endl;
            delete servicedata_and_cbs;
            return;
        }
        VSOMEIP_DEBUG << __func__ << " TLSA SERVICE RESPONSE RECEIVE";
        servicedata_and_cbs->record_timestamp_callback_(servicedata_and_cbs->its_unicast_.to_uint(), time_metric::TLSA_SERVICE_RESPONSE_RECEIVE_);

        unsigned char* copy = new unsigned char[_alen];
        memcpy(copy, _abuf, _alen);
        tlsa_reply* tlsareply;
        if ((parse_tlsa_reply(copy, _alen, &tlsareply)) != ARES_SUCCESS) {
            std::cerr << "Parsing service TLSA reply failed" << std::endl;
            delete servicedata_and_cbs;
            delete[] copy;
            delete_tlsa_reply(tlsareply);
            return;
        }

        tlsa_reply* tlsa_reply_ptr = tlsareply;
        while (tlsa_reply_ptr != nullptr) {
            servicedata_and_cbs->add_publisher_certificate_callback_(servicedata_and_cbs->ipv4_address_, servicedata_and_cbs->service_, servicedata_and_cbs->instance_, servicedata_and_cbs->convert_der_to_pem_callback_(tlsa_reply_ptr->certificate_association_data_));
            servicedata_and_cbs->validate_subscribe_ack_and_verify_signature_callback_(servicedata_and_cbs->ipv4_address_, servicedata_and_cbs->service_, servicedata_and_cbs->instance_, servicedata_and_cbs->major_);
            tlsa_reply_ptr = tlsa_reply_ptr->tlsa_reply_next_;
        }
        delete servicedata_and_cbs;
        delete[] copy;
        delete_tlsa_reply(tlsareply);
    }

    void client_tlsa_resolve_callback(void* _data, int _status, int _timeouts,
                unsigned char* _abuf, int _alen) {
        client_data_and_cbs* clientdata_and_cbs = reinterpret_cast<client_data_and_cbs*>(_data);
        
        if (_status) {
            std::cerr << __func__ << " Bad DNS response" << std::endl;
            delete clientdata_and_cbs;
            return;
        }

        if (_timeouts) {
            std::cerr << __func__ << " DNS request timeout" << std::endl;
            delete clientdata_and_cbs;
            return;
        }
        VSOMEIP_DEBUG << __func__ << " TLSA CLIENT RESPONSE RECEIVE";
        clientdata_and_cbs->record_timestamp_callback_(clientdata_and_cbs->unverified_client_ipv4_address_.to_uint(), time_metric::TLSA_CLIENT_RESPONSE_RECEIVE_);

        unsigned char* copy = new unsigned char[_alen];
        memcpy(copy, _abuf, _alen);
        tlsa_reply* tlsareply;
        if ((parse_tlsa_reply(copy, _alen, &tlsareply)) != ARES_SUCCESS) {
            std::cerr << "Parsing client TLSA reply failed" << std::endl;
            delete clientdata_and_cbs;
            delete[] copy;
            delete_tlsa_reply(tlsareply);
            return;
        }

        tlsa_reply* tlsa_reply_ptr = tlsareply;
        while (tlsa_reply_ptr != nullptr) {
            clientdata_and_cbs->add_subscriber_certificate_callback_(clientdata_and_cbs->client_, clientdata_and_cbs->ipv4_address_, clientdata_and_cbs->service_, clientdata_and_cbs->instance_, clientdata_and_cbs->convert_der_to_pem_callback_(tlsa_reply_ptr->certificate_association_data_));
            tlsa_reply_ptr = tlsa_reply_ptr->tlsa_reply_next_;
        }
        clientdata_and_cbs->client_tlsa_record_condition_variable_.notify_one();
        delete clientdata_and_cbs;
        delete[] copy;
        delete_tlsa_reply(tlsareply);
    }

    void tlsa_resolver::request_service_tlsa_record(void* _service_data) {
        service_data_and_cbs* servicedata_and_cbs = reinterpret_cast<service_data_and_cbs*>(_service_data);
        std::stringstream request;
        request << ATTRLEAFBRANCH;
        request << "minor0x" << std::hex << std::setw(8) << std::setfill('0') << (int) servicedata_and_cbs->minor_;
        request << ".";
        request << "major0x" << std::hex << std::setw(2) << std::setfill('0') << (int) servicedata_and_cbs->major_;
        request << ".";
        request << "instance0x" << std::hex << std::setw(4) << std::setfill('0') << (int) servicedata_and_cbs->instance_;
        request << ".";
        request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << (int) servicedata_and_cbs->service_;
        request << ".";
        request << SERVICE_PARENTDOMAIN;
        VSOMEIP_DEBUG << __func__ << " TLSA SERVICE REQUEST SEND";
        servicedata_and_cbs->record_timestamp_callback_(servicedata_and_cbs->its_unicast_.to_uint(), time_metric::TLSA_SERVICE_REQUEST_SEND_);
        dns_resolver_->resolve(request.str().c_str(), C_IN, T_TLSA, service_tlsa_resolve_callback, _service_data);
    }

    void tlsa_resolver::request_client_tlsa_record(void* _client_data) {
        client_data_and_cbs* clientdata_and_cbs = reinterpret_cast<client_data_and_cbs*>(_client_data);
        std::stringstream request;
        request << ATTRLEAFBRANCH;
        request << "major0x" << std::hex << std::setw(2) << std::setfill('0') << (int) clientdata_and_cbs->major_;
        request << ".";
        request << "instance0x" << std::hex << std::setw(4) << std::setfill('0') << (int) clientdata_and_cbs->instance_;
        request << ".";
        request << "service0x" << std::hex << std::setw(4) << std::setfill('0') << (int) clientdata_and_cbs->service_;
        request << ".";
        request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << (int) clientdata_and_cbs->client_;
        request << ".";
        request << CLIENT_PARENTDOMAIN;
        VSOMEIP_DEBUG << __func__ << " TLSA CLIENT REQUEST SEND";
        clientdata_and_cbs->record_timestamp_callback_(clientdata_and_cbs->unverified_client_ipv4_address_.to_uint(), time_metric::TLSA_CLIENT_REQUEST_SEND_);
        dns_resolver_->resolve(request.str().c_str(), C_IN, T_TLSA, client_tlsa_resolve_callback, _client_data);
    }
} /* end namespace vsomeip_v3 */