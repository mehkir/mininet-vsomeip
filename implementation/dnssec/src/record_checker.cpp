//
// Created by mehkir on 03.08.22.
//

#include "../include/record_checker.hpp"
#include "../include/logger.hpp"
#include "../include/parse_svcb_reply.hpp"
#include "../include/parse_tlsa_reply.hpp"
#include <arpa/nameser.h>
#include <netinet/in.h>
#include <string>
#include <vsomeip/constants.hpp>
#include <vsomeip/internal/logger.hpp>

namespace vsomeip_v3 {

    record_checker::record_checker() : dns_resolver_(dns_resolver::getInstance()) {
        dns_resolver_->initialize(DNS_SERVER_IP);
    }

    bool record_checker::is_svcb_valid() {
        return true;
    }

    void LocalResolverCallback(void *data, int status, int timeouts,
                unsigned char *abuf, int alen) {
        LOG_DEBUG("LocalResolverCallback is called")
        auto result = reinterpret_cast<LocalServiceData*>(data);
        
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
            std::cout << "record checker\n" << *svcbReplyPtr << std::endl;
            svcbReplyPtr->getSVCBKey(INSTANCE);
            svcbReplyPtr->getSVCBKey(MAJOR_VERSION);
            result->callback(result->client, result->service, result->instance, result->eventGroup, result->major, result->event);
            svcbReplyPtr = svcbReplyPtr->svcbReplyNext;
        }
        delete_svcb_reply(svcbReply);
        
        delete result;
        delete[] copy;
    }

    void record_checker::resolveLocalSomeipService(LocalServiceData* serviceData) {
        std::string request(ATTRLEAFBRANCH);
        request.append(std::to_string(serviceData->service));
        request.append(".");
        request.append(PARENTDOMAIN);
        dns_resolver_->resolve(request.c_str(), C_IN, T_SVCB, LocalResolverCallback, serviceData);
    }

    void svcb_resolve_callback(void *data, int status, int timeouts,
                unsigned char *abuf, int alen) {
        LOG_DEBUG("svcb_resolve_callback is called")
        remote_service_data* remote_service_data_ = reinterpret_cast<remote_service_data*>(data);
        
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
            std::cout << "record checker\n" << *svcbReplyPtr << std::endl;
            std::string reliable_address = "", unreliable_address = "";
            uint16_t reliable_port = 0, unreliable_port = 0;
            int l4protocol = std::stoi(svcbReplyPtr->getSVCBKey(L4PROTOCOL));
            switch (l4protocol)
            {
            case IPPROTO_UDP:
                unreliable_address = svcbReplyPtr->ipv4AddressString;
                unreliable_port = svcbReplyPtr->port;
                remote_service_data_->remote_ip_address = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            case IPPROTO_TCP:
                reliable_address = svcbReplyPtr->ipv4AddressString;
                reliable_port = svcbReplyPtr->port;
                remote_service_data_->remote_ip_address = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            default:
                break;
            }
            remote_service_data_->offer_callback(remote_service_data_->service,
                            std::stoi(svcbReplyPtr->getSVCBKey(INSTANCE),0,16),
                            std::stoi(svcbReplyPtr->getSVCBKey(MAJOR_VERSION),0,16),
                            std::stoi(svcbReplyPtr->getSVCBKey(MINOR_VERSION),0,16),
                            DEFAULT_TTL, reliable_address, reliable_port,
                            unreliable_address, unreliable_port);
            //Fill with concrete values in case instance, major and minor were not specified
            remote_service_data_->instance = std::stoi(svcbReplyPtr->getSVCBKey(INSTANCE),0,16);
            remote_service_data_->major = std::stoi(svcbReplyPtr->getSVCBKey(MAJOR_VERSION),0,16);
            remote_service_data_->minor = std::stoi(svcbReplyPtr->getSVCBKey(MINOR_VERSION),0,16);
            //Assemble TLSA QNAME
            std::stringstream tlsa_request;
            tlsa_request << ATTRLEAFBRANCH;
            tlsa_request << "minor" << svcbReplyPtr->getSVCBKey(MINOR_VERSION);
            tlsa_request << ".";
            tlsa_request << "major" << svcbReplyPtr->getSVCBKey(MAJOR_VERSION);
            tlsa_request << ".";
            tlsa_request << "instance" << svcbReplyPtr->getSVCBKey(INSTANCE);
            tlsa_request << ".";
            tlsa_request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << remote_service_data_->service;
            tlsa_request << ".";
            tlsa_request << PARENTDOMAIN;
            remote_service_data_->request_tlsa_record_callback(remote_service_data_, tlsa_request.str());

            svcbReplyPtr = svcbReplyPtr->svcbReplyNext;
        }
        delete_svcb_reply(svcbReply);
        
        //delete remote_service_data_;
        delete[] copy;
    }

    void tlsa_resolve_callback(void *data, int status, int timeouts,
                unsigned char *abuf, int alen) {
        remote_service_data* result = reinterpret_cast<remote_service_data*>(data);
        
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
            std::cout << "record checker\n" << *tlsa_reply_ptr << std::endl;
            result->request_cache_callback(result->remote_ip_address, result->service, result->instance, tlsa_reply_ptr->certificate_association_data);
            tlsa_reply_ptr = tlsa_reply_ptr->tlsaReplyNext;
        }
        delete_tlsa_reply(tlsa_reply);
        
        delete result;
        delete[] copy;
    }

    void record_checker::request_svcb_record(remote_service_data* service_data) {
        std::stringstream request;
        request << ATTRLEAFBRANCH;
        if (service_data->minor != ANY_MINOR) {
            request << "minor0x" << std::hex << std::setw(8) << std::setfill('0') << service_data->minor;
            request << ".";
        }
        if (service_data->major != ANY_MAJOR) {
            request << "major0x" << std::hex << std::setw(2) << std::setfill('0') << service_data->major;
            request << ".";
        }
        if (service_data->instance != ANY_INSTANCE) {
            request << "instance0x" << std::hex << std::setw(4) << std::setfill('0') << service_data->instance;
            request << ".";
        }
        request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << service_data->service;
        request << ".";
        request << PARENTDOMAIN;
        dns_resolver_->resolve(request.str().c_str(), C_IN, T_SVCB, svcb_resolve_callback, service_data);
    }

    void record_checker::request_tlsa_record(void* service_data, std::string tlsa_request) {
        dns_resolver_->resolve(tlsa_request.c_str(), C_IN, T_TLSA, tlsa_resolve_callback, reinterpret_cast<remote_service_data*>(service_data));
    }

    bool record_checker::is_tlsa_valid() {
        return true;
    }

} // namespace vsomeip_v3