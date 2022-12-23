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

namespace vsomeip_v3 {

    record_checker::record_checker() : dnsResolver(dns_resolver::getInstance()) {
        dnsResolver->initialize(DNS_SERVER_IP);
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
        dnsResolver->resolve(request.c_str(), C_IN, T_SVCB, LocalResolverCallback, serviceData);
    }

    void RemoteResolverCallback(void *data, int status, int timeouts,
                unsigned char *abuf, int alen) {
        LOG_DEBUG("RemoteResolverCallback is called")
        auto result = reinterpret_cast<RemoteServiceData*>(data);
        
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
            if (result->instance == std::stoi(svcbReplyPtr->getSVCBKey(INSTANCE))
                && (result->major == ANY_MAJOR || std::stoi(svcbReplyPtr->getSVCBKey(MAJOR_VERSION)) == result->major)
                && (result->minor == ANY_MINOR || std::stoi(svcbReplyPtr->getSVCBKey(MINOR_VERSION)) == result->minor)) {
                    std::string reliable_address = "", unreliable_address = "";
                    uint16_t reliable_port = 0, unreliable_port = 0;
                    int l4protocol = std::stoi(svcbReplyPtr->getSVCBKey(L4PROTOCOL));
                    switch (l4protocol)
                    {
                    case IPPROTO_UDP:
                        unreliable_address = svcbReplyPtr->ipv4AddressString;
                        unreliable_port = svcbReplyPtr->port;
                        break;
                    case IPPROTO_TCP:
                        reliable_address = svcbReplyPtr->ipv4AddressString;
                        reliable_port = svcbReplyPtr->port;
                        break;
                    default:
                        break;
                    }
                    result->callback(result->service, result->instance, std::stoi(svcbReplyPtr->getSVCBKey(MAJOR_VERSION)),
                                    std::stoi(svcbReplyPtr->getSVCBKey(MINOR_VERSION)), DEFAULT_TTL, reliable_address, reliable_port,
                                    unreliable_address, unreliable_port);
                }
            svcbReplyPtr = svcbReplyPtr->svcbReplyNext;
        }
        delete_svcb_reply(svcbReply);
        
        delete result;
        delete[] copy;
    }

    void RemoteCertificateResolverCallback(void *data, int status, int timeouts,
                unsigned char *abuf, int alen) {
        auto result = reinterpret_cast<RemoteServiceData*>(data);
        
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
            result->certificate_callback(result->ip_address, result->service, result->instance, tlsa_reply_ptr->certificate_association_data);
            tlsa_reply_ptr = tlsa_reply_ptr->tlsaReplyNext;
        }
        delete_tlsa_reply(tlsa_reply);
        
        delete result;
        delete[] copy;
    }

    void record_checker::resolveRemoteSomeipService(RemoteServiceData* serviceData) {
        std::string request(ATTRLEAFBRANCH);
        request.append(std::to_string(serviceData->service));
        request.append(".");
        request.append(PARENTDOMAIN);
        dnsResolver->resolve(request.c_str(), C_IN, T_SVCB, RemoteResolverCallback, serviceData);
    }

    void record_checker::resolveRemoteSomeipServiceCertificate(RemoteServiceData* serviceData) {
        std::string request(ATTRLEAFBRANCH);
        request.append(std::to_string(serviceData->service));
        request.append(".");
        request.append(PARENTDOMAIN);
        dnsResolver->resolve(request.c_str(), C_IN, T_TLSA, RemoteCertificateResolverCallback, serviceData);
    }

    bool record_checker::is_tlsa_valid() {
        return true;
    }

} // namespace vsomeip_v3