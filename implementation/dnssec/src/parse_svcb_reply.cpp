//
// Created by mehkir on 21.09.22.
//

#include <ares.h>
#include <ares_dns.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>
#include <sstream>
#include "../include/parse_svcb_reply.hpp"
#include "../include/logger.hpp"

void delete_svcb_reply(SVCB_Reply* svcbReply) {
    SVCB_Reply* svcbReplyCurrent = svcbReply;
    SVCB_Reply* svcbReplySuccessor = nullptr;
    while (svcbReplyCurrent != nullptr) {
        LOG_DEBUG("Delete SVCB reply: " << svcbReplyCurrent->hostname)
        SVCB_Key *svcbKeyCurrent = svcbReplyCurrent->svcbKey;
        SVCB_Key *svcbKeySuccessor = nullptr;
        while (svcbKeyCurrent != nullptr) {
            svcbKeySuccessor = svcbKeyCurrent->svcbKeyNext;
            LOG_DEBUG("\tDelete key\t" << svcbKeyCurrent->keyNum)
            delete(svcbKeyCurrent);
            svcbKeyCurrent = svcbKeySuccessor;
        }
        svcbReplySuccessor = svcbReplyCurrent->svcbReplyNext;
        delete (svcbReplyCurrent);
        svcbReplyCurrent = svcbReplySuccessor;
    }
}

int parse_svcb_reply (const unsigned char *abuf, int alen, SVCB_Reply** pSvcbReply) {
    unsigned int qdcount, ancount, i;
    const unsigned char *aptr, *vptr;
    int status, rr_type, rr_class, rr_len;
    long len;
    char *hostname = NULL, *rr_name = NULL, *target_name = NULL;
    SVCB_Reply *svcbReply = nullptr;
    SVCB_Reply *currSvcbReply = nullptr;

    /* Give up if abuf doesn't have room for a header. */
    if (alen < HFIXEDSZ)
        return ARES_EBADRESP;

    /* Fetch the question and answer count from the header. */
    qdcount = DNS_HEADER_QDCOUNT(abuf);
    ancount = DNS_HEADER_ANCOUNT(abuf);
    if (qdcount != 1)
        return ARES_EBADRESP;
    if (ancount == 0)
        return ARES_ENODATA;

    /* Expand the name from the question, and skip past the question. */
    aptr = abuf + HFIXEDSZ;
    status = ares_expand_name(aptr, abuf, alen, &hostname, &len);
    if (status != ARES_SUCCESS)
        return status;

    if (aptr + len + QFIXEDSZ > abuf + alen) {
        ares_free_string(hostname);
        return ARES_EBADRESP;
    }
    aptr += len + QFIXEDSZ;

    /* Examine each answer resource record (RR) in turn. */
    for (i = 0; i < ancount; i++) {
        /* Decode the RR up to the data field. */
        status = ares_expand_name(aptr, abuf, alen, &rr_name, &len);
        if (status != ARES_SUCCESS) {
            break;
        }
        aptr += len;
        if (aptr + RRFIXEDSZ > abuf + alen) {
            status = ARES_EBADRESP;
            break;
        }
        rr_type = DNS_RR_TYPE (aptr);
        rr_class = DNS_RR_CLASS (aptr);
        rr_len = DNS_RR_LEN (aptr);
        aptr += RRFIXEDSZ;
        if (aptr + rr_len > abuf + alen) {
            status = ARES_EBADRESP;
            break;
        }
        if (rr_class == C_IN && rr_type == T_SVCB) {
            vptr = aptr;
            SVCB_Key* svcbKey = nullptr;
            if (svcbReply == nullptr) {
                svcbReply = new SVCB_Reply;
                currSvcbReply = svcbReply;
            } else {
                currSvcbReply->svcbReplyNext = new SVCB_Reply;
                currSvcbReply = currSvcbReply->svcbReplyNext;
            }
            currSvcbReply->hostname = hostname;
            LOG_DEBUG("Hostname " << hostname);
            currSvcbReply->svcPriority = DNS__16BIT(vptr);
            LOG_DEBUG("SvcPriority " << currSvcbReply->svcPriority)
            vptr += sizeof(unsigned short);
            status = ares_expand_name(vptr, abuf, alen, &target_name, &len);
            if (status != ARES_SUCCESS) {
                break;
            }
            LOG_DEBUG("Target " << currSvcbReply->target)
            currSvcbReply->target = std::string(target_name) == "" ? "." : std::string(target_name);
            vptr += len;
            ares_free_string(target_name);
            target_name = NULL;
            while (vptr < aptr + rr_len) {
                switch (DNS__16BIT(vptr)) {
                    case SVCB_PARAM_TYPE::MANDATORY:
                        LOG_DEBUG("mandatory")
                        break;
                    case SVCB_PARAM_TYPE::ALPN:
                        LOG_DEBUG("alpn")
                        break;
                    case SVCB_PARAM_TYPE::NO_DEFAULT_ALPN:
                        LOG_DEBUG("no-default-alpn")
                        break;
                    case SVCB_PARAM_TYPE::PORT: {
                        LOG_DEBUG("port")
                        vptr += sizeof(unsigned short);
                        int length = DNS__16BIT(vptr);
                        LOG_DEBUG("Length " << length)
                        vptr += sizeof(unsigned short);
                        LOG_DEBUG("Port number " << DNS__16BIT(vptr))
                        currSvcbReply->port = DNS__16BIT(vptr);
                        vptr += length * sizeof(unsigned char);
                    }
                        break;
                    case SVCB_PARAM_TYPE::IPV4HINT: {
                        LOG_DEBUG("ipv4hint")
                        vptr += sizeof(unsigned short);
                        int length = DNS__16BIT(vptr);
                        LOG_DEBUG("Length " << length)
                        vptr += sizeof(unsigned short);                        
                        currSvcbReply->ipv4AddressNumerical = DNS__32BIT(vptr);
                        LOG_DEBUG("IP address (numerical) " << currSvcbReply->ipv4AddressNumerical)
                        std::stringstream sstream;
                        sstream << ((currSvcbReply->ipv4AddressNumerical & 0xff000000) >> 24) << "."
                                << ((currSvcbReply->ipv4AddressNumerical & 0x00ff0000) >> 16) << "."
                                << ((currSvcbReply->ipv4AddressNumerical & 0x0000ff00) >> 8) << "."
                                << (currSvcbReply->ipv4AddressNumerical & 0x000000ff);
                        currSvcbReply->ipv4AddressString = sstream.str();
                        LOG_DEBUG("IP address (string) " << currSvcbReply->ipv4AddressString)
                        vptr += length * sizeof(unsigned char);
                    }
                        break;
                    case SVCB_PARAM_TYPE::ECH:
                        LOG_DEBUG("ech")
                        break;
                    case SVCB_PARAM_TYPE::IPV6HINT:
                        LOG_DEBUG("ipv6hint")
                        break;
                    default:
                        if (DNS__16BIT(vptr) >= SVCB_PARAM_TYPE::LOWERKEY
                            || DNS__16BIT(vptr) <= SVCB_PARAM_TYPE::UPPERKEY) {
                            LOG_DEBUG("key")
                            uint16_t keyNumber = DNS__16BIT(vptr);
                            LOG_DEBUG("Key number " << keyNumber)
                            vptr += sizeof(unsigned short);
                            int length = DNS__16BIT(vptr);
                            LOG_DEBUG("Length " << length)
                            vptr += sizeof(unsigned short);
                            std::string keyValue = std::string((char*) vptr,length);
                            LOG_DEBUG("Key value " << keyValue)
                            vptr += length * sizeof(unsigned char);
                            if (currSvcbReply->svcbKey == nullptr) {
                                currSvcbReply->svcbKey = new SVCB_Key;
                                svcbKey = currSvcbReply->svcbKey;
                            } else {
                                svcbKey->svcbKeyNext = new SVCB_Key;
                                svcbKey = svcbKey->svcbKeyNext;
                            }
                            svcbKey->keyNum = keyNumber;
                            svcbKey->keyVal = keyValue;
                        }
                }
            }
        }
        /* Don't lose memory in the next iteration */
        ares_free_string(rr_name);
        rr_name = NULL;

        /* Move on to the next record */
        aptr += rr_len;
    }
    ares_free_string(hostname);
    hostname = NULL;
    *pSvcbReply = svcbReply;
    return ARES_SUCCESS;
}