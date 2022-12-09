//
// Created by mehkir on 03.08.22.
//

#include "../include/record_checker.hpp"
#include "../include/logger.hpp"
#include "../include/parse_svcb_reply.hpp"
#include <arpa/nameser.h>
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
                    result->callback(result->service, result->instance, std::stoi(svcbReplyPtr->getSVCBKey(MAJOR_VERSION)),
                                    std::stoi(svcbReplyPtr->getSVCBKey(MINOR_VERSION)), 0xFFFFFF, "", 0,
                                    svcbReplyPtr->ipv4AddressString, svcbReplyPtr->port);
                }
            svcbReplyPtr = svcbReplyPtr->svcbReplyNext;
        }
        delete_svcb_reply(svcbReply);
        
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

    bool record_checker::is_tlsa_valid() {
        return true;
    }

} // namespace vsomeip_v3