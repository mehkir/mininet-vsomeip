#include "../include/svcb_resolver.hpp"
#include "../include/logger.hpp"
#include "../include/parse_svcb_reply.hpp"
#include "../../timestamps/include/timestamp_collector.hpp"
#include <arpa/nameser.h>
#include <netinet/in.h>
#include <string>
#include <iomanip>

namespace vsomeip_v3 {
    svcb_resolver::svcb_resolver() : dns_resolver_(dns_resolver::get_instance()) {
        dns_resolver_->initialize(DNS_SERVER_IP);
    }

    svcb_resolver::~svcb_resolver() {
    }

    void svcb_resolve_callback(void *data, int status, int timeouts,
                unsigned char *abuf, int alen) {
        LOG_DEBUG("svcb_resolve_callback is called")
        service_data_and_cbs* servicedata_and_cbs = reinterpret_cast<service_data_and_cbs*>(data);
        
        if (status) {
            std::cout << "Bad DNS response" << std::endl;
            return;
        }

        unsigned char* copy = new unsigned char[alen];
        memcpy(copy, abuf, alen);
        svcb_reply* svcbreply;
        if ((parse_svcb_reply(copy, alen, &svcbreply)) != ARES_SUCCESS) {
            std::cout << "Parsing SVCB reply failed" << std::endl;
            delete_svcb_reply(svcbreply);
            return;
        }
        
        svcb_reply* svcb_reply_ptr = svcbreply;
        while (svcb_reply_ptr != nullptr) {
            //std::cout << "svcb_resolver\n" << *svcb_reply_ptr << std::endl;
            std::string reliable_address = "", unreliable_address = "";
            uint16_t reliable_port = 0, unreliable_port = 0;
            int l4protocol = std::stoi(svcb_reply_ptr->get_svcb_key(L4PROTOCOL));
            switch (l4protocol)
            {
            case IPPROTO_UDP:
                unreliable_address = svcb_reply_ptr->ipv4_address_string_;
                unreliable_port = svcb_reply_ptr->port_;
                servicedata_and_cbs->ipv4_address_ = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            case IPPROTO_TCP:
                reliable_address = svcb_reply_ptr->ipv4_address_string_;
                reliable_port = svcb_reply_ptr->port_;
                servicedata_and_cbs->ipv4_address_ = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            default:
                break;
            }
            //Fill with concrete values in case instance, major and minor were not specified
            servicedata_and_cbs->instance_ = std::stoi(svcb_reply_ptr->get_svcb_key(INSTANCE),0,16);
            servicedata_and_cbs->major_ = std::stoi(svcb_reply_ptr->get_svcb_key(MAJOR_VERSION),0,16);
            servicedata_and_cbs->minor_ = std::stoi(svcb_reply_ptr->get_svcb_key(MINOR_VERSION),0,16);
            servicedata_and_cbs->add_svcb_entry_cache_callback_(servicedata_and_cbs->service, servicedata_and_cbs->instance, servicedata_and_cbs->major_, servicedata_and_cbs->minor, l4protocol, servicedata_and_cbs->ip_address, svcb_reply_ptr->port_);
            servicedata_and_cbs->verify_service_info_callback_(servicedata_and_cbs->service, servicedata_and_cbs->instance, servicedata_and_cbs->major_, servicedata_and_cbs->minor);
            //Assemble TLSA QNAME
            std::stringstream tlsa_request;
            tlsa_request << ATTRLEAFBRANCH;
            tlsa_request << "minor" << svcb_reply_ptr->get_svcb_key(MINOR_VERSION);
            tlsa_request << ".";
            tlsa_request << "major" << svcb_reply_ptr->get_svcb_key(MAJOR_VERSION);
            tlsa_request << ".";
            tlsa_request << "instance" << svcb_reply_ptr->get_svcb_key(INSTANCE);
            tlsa_request << ".";
            tlsa_request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << servicedata_and_cbs->service;
            tlsa_request << ".";
            tlsa_request << PARENTDOMAIN;
            servicedata_and_cbs->request_tlsa_record_callback_(servicedata_and_cbs, tlsa_request.str());

            svcb_reply_ptr = svcb_reply_ptr->svcb_reply_next_;
        }
        delete_svcb_reply(svcbreply);
        
        //delete servicedata_and_cbs;
        delete[] copy;
    }

    void svcb_resolver::request_svcb_record(service_data_and_cbs* _service_data_and_cbs) {
        std::stringstream request;
        request << ATTRLEAFBRANCH;
        if (_service_data_and_cbs->minor != ANY_MINOR) {
            request << "minor0x" << std::hex << std::setw(8) << std::setfill('0') << _service_data_and_cbs->minor;
            request << ".";
        }
        if (_service_data_and_cbs->major != ANY_MAJOR) {
            request << "major0x" << std::hex << std::setw(2) << std::setfill('0') << _service_data_and_cbs->major;
            request << ".";
        }
        if (_service_data_and_cbs->instance != ANY_INSTANCE) {
            request << "instance0x" << std::hex << std::setw(4) << std::setfill('0') << _service_data_and_cbs->instance;
            request << ".";
        }
        request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << _service_data_and_cbs->service;
        request << ".";
        request << PARENTDOMAIN;
        dns_resolver_->resolve(request.str().c_str(), C_IN, T_SVCB, svcb_resolve_callback, _service_data_and_cbs);
    }
} /* end namespace vsomeip_v3 */