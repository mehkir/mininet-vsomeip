//
// Created by mehkir on 03.08.22.
//

#include "../include/record_checker.hpp"
#include "../include/logger.hpp"
#include "../include/parse_svcb_reply.hpp"
#include <arpa/nameser.h>
#include <string>

namespace vsomeip_v3 {

    void ResolverCallback(void *data, int status, int timeouts,
                unsigned char *abuf, int alen) {
        LOG_DEBUG("ResolverCallback is called")
        auto result = reinterpret_cast<ServiceData*>(data);
        
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
        //processRequest(result);
    }


    record_checker::record_checker() : dnsResolver(dns_resolver::getInstance()) {
        dnsResolver->initialize(DNS_SERVER_IP);
    }

    bool record_checker::is_svcb_valid() {
        return true;
    }

    void record_checker::resolveSomeipService(ServiceData* serviceData) {
        std::string request("_someip._udp.");
        request.append(std::to_string(serviceData->service));
        request.append(".service.");
        dnsResolver->resolve(request.c_str(), C_IN, T_SVCB, ResolverCallback, serviceData);
    }

    bool record_checker::is_tlsa_valid() {
        return true;
    }

} // namespace vsomeip_v3