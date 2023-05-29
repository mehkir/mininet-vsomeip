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
        service_data_and_cbs* service_data_and_cbs_ = reinterpret_cast<service_data_and_cbs*>(data);
        
        if (status) {
            std::cout << "Bad DNS response" << std::endl;
            return;
        }

        unsigned char* copy = new unsigned char[alen];
        memcpy(copy, abuf, alen);
        SVCB_Reply* svcbReply;
        if ((parse_svcb_reply(copy, alen, &svcbReply)) != ARES_SUCCESS) {
            std::cout << "Parsing SVCB reply failed" << std::endl;
            delete_svcb_reply(svcbReply);
            return;
        }
        
        SVCB_Reply* svcbReplyPtr = svcbReply;
        while (svcbReplyPtr != nullptr) {
            //std::cout << "svcb_resolver\n" << *svcbReplyPtr << std::endl;
            std::string reliable_address = "", unreliable_address = "";
            uint16_t reliable_port = 0, unreliable_port = 0;
            int l4protocol = std::stoi(svcbReplyPtr->getSVCBKey(L4PROTOCOL));
            switch (l4protocol)
            {
            case IPPROTO_UDP:
                unreliable_address = svcbReplyPtr->ipv4AddressString;
                unreliable_port = svcbReplyPtr->port;
                service_data_and_cbs_->ip_address = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            case IPPROTO_TCP:
                reliable_address = svcbReplyPtr->ipv4AddressString;
                reliable_port = svcbReplyPtr->port;
                service_data_and_cbs_->ip_address = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            default:
                break;
            }
            //Fill with concrete values in case instance, major and minor were not specified
            service_data_and_cbs_->instance = std::stoi(svcbReplyPtr->getSVCBKey(INSTANCE),0,16);
            service_data_and_cbs_->major = std::stoi(svcbReplyPtr->getSVCBKey(MAJOR_VERSION),0,16);
            service_data_and_cbs_->minor = std::stoi(svcbReplyPtr->getSVCBKey(MINOR_VERSION),0,16);
            service_data_and_cbs_->add_svcb_entry_cache_callback_(service_data_and_cbs_->service, service_data_and_cbs_->instance, service_data_and_cbs_->major, service_data_and_cbs_->minor, l4protocol, service_data_and_cbs_->ip_address, svcbReplyPtr->port);
            service_data_and_cbs_->verify_service_info_callback_(service_data_and_cbs_->service, service_data_and_cbs_->instance, service_data_and_cbs_->major, service_data_and_cbs_->minor);
            //Assemble TLSA QNAME
            std::stringstream tlsa_request;
            tlsa_request << ATTRLEAFBRANCH;
            tlsa_request << "minor" << svcbReplyPtr->getSVCBKey(MINOR_VERSION);
            tlsa_request << ".";
            tlsa_request << "major" << svcbReplyPtr->getSVCBKey(MAJOR_VERSION);
            tlsa_request << ".";
            tlsa_request << "instance" << svcbReplyPtr->getSVCBKey(INSTANCE);
            tlsa_request << ".";
            tlsa_request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << service_data_and_cbs_->service;
            tlsa_request << ".";
            tlsa_request << PARENTDOMAIN;
            service_data_and_cbs_->request_tlsa_record_callback_(service_data_and_cbs_, tlsa_request.str());

            svcbReplyPtr = svcbReplyPtr->svcbReplyNext;
        }
        delete_svcb_reply(svcbReply);
        
        //delete service_data_and_cbs_;
        delete[] copy;
    }

    void svcb_resolver::request_svcb_record(service_data_and_cbs* service_data_and_cbs_) {
        std::stringstream request;
        request << ATTRLEAFBRANCH;
        if (service_data_and_cbs_->minor != ANY_MINOR) {
            request << "minor0x" << std::hex << std::setw(8) << std::setfill('0') << service_data_and_cbs_->minor;
            request << ".";
        }
        if (service_data_and_cbs_->major != ANY_MAJOR) {
            request << "major0x" << std::hex << std::setw(2) << std::setfill('0') << service_data_and_cbs_->major;
            request << ".";
        }
        if (service_data_and_cbs_->instance != ANY_INSTANCE) {
            request << "instance0x" << std::hex << std::setw(4) << std::setfill('0') << service_data_and_cbs_->instance;
            request << ".";
        }
        request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << service_data_and_cbs_->service;
        request << ".";
        request << PARENTDOMAIN;
        dns_resolver_->resolve(request.str().c_str(), C_IN, T_SVCB, svcb_resolve_callback, service_data_and_cbs_);
    }
} /* end namespace vsomeip_v3 */