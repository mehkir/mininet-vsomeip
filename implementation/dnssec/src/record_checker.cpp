//
// Created by mehkir on 03.08.22.
//

#include "../include/record_checker.hpp"
#include "../include/logger.hpp"
#include "../include/parse_svcb_reply.hpp"
#include <arpa/nameser.h>
#include <string.h>

    static int processRequest(vsomeip_v3::SearchResult* searchResult) {
        if (searchResult->status_ != ARES_SUCCESS) {
            std::cout << "Query failed" << std::endl;
            return 1;
        }

        LOG_DEBUG("Result Done: " << searchResult->done_)
        LOG_DEBUG("Result Timeouts: " << searchResult->timeouts_)
        LOG_DEBUG("Response size: " << searchResult->data_.size())

        SVCB_Reply* svcbReply;
        if ((parse_svcb_reply(searchResult->data_.data(), searchResult->data_.size(), &svcbReply)) != ARES_SUCCESS) {
            std::cout << "Parsing SVCB reply failed" << std::endl;
            return 1;
        }
        SVCB_Reply* svcbReplyPtr = svcbReply;
        while (svcbReplyPtr != nullptr) {
            std::cout << *svcbReplyPtr << std::endl;
            svcbReplyPtr = svcbReplyPtr->svcbReplyNext;
        }
        //std::cout << "key65280=" << svcbReply->getSVCBKey(65280) << ", " <<  "key65282=" << svcbReply->getSVCBKey(65282) << std::endl;
        delete_svcb_reply(svcbReply);
        delete(searchResult);
        return ARES_SUCCESS;
    }

    void SearchCallback(void *data, int status, int timeouts,
                    unsigned char *abuf, int alen) {
        auto result = reinterpret_cast<vsomeip_v3::SearchResult*>(data);
        result->done_ = true;
        result->status_ = status;
        result->timeouts_ = timeouts;
        std::vector<vsomeip_v3::byte> buffer;
        buffer.assign(abuf, abuf + alen);

        unsigned char* copy = (unsigned char*)malloc(alen);
        memcpy(copy, abuf, alen);

        LOG_DEBUG("SearchCallback(" << result->done_ << ")")

        SVCB_Reply* svcbReply;
        if ((parse_svcb_reply(copy, alen, &svcbReply)) != ARES_SUCCESS) {
            std::cout << "Parsing SVCB reply failed" << std::endl;
        }
        SVCB_Reply* svcbReplyPtr = svcbReply;
        while (svcbReplyPtr != nullptr) {
            std::cout << *svcbReplyPtr << std::endl;
            svcbReplyPtr = svcbReplyPtr->svcbReplyNext;
        }
        delete_svcb_reply(svcbReply);
        
        free(result);
        free(copy);
        //processRequest(result);
    }

namespace vsomeip_v3 {

    record_checker::record_checker() : dnsResolver(dns_resolver::getInstance()), callback(SearchCallback) {
        dnsResolver->initialize(0xAC110002);
    }

    bool record_checker::is_svcb_valid() {
        SearchResult* result = (SearchResult*) malloc(2*sizeof(int)+sizeof(bool));
        dnsResolver->resolve("_someip._udp.1.service.", C_IN, T_SVCB, callback, result);
        return true;
    }

    bool record_checker::is_tlsa_valid() {

        return true;
    }

} // namespace vsomeip_v3