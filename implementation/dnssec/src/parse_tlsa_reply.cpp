//
// Created by mehkir on 12.09.22.
//

#include <ares.h>
#include <ares_dns.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>
#include <sstream>
#include "../include/parse_tlsa_reply.hpp"
#include "../include/logger.hpp"

void delete_tlsa_reply(TLSA_Reply* tlsaReply) {
    TLSA_Reply* tlsaReplyCurrent = tlsaReply;
    TLSA_Reply* tlsaReplySuccessor = nullptr;
    while (tlsaReplyCurrent != nullptr) {
        LOG_DEBUG("Delete TLSA reply: " << tlsaReplyCurrent->hostname)
        tlsaReplySuccessor = tlsaReplyCurrent->tlsaReplyNext;
        delete (tlsaReplyCurrent);
        tlsaReplyCurrent = tlsaReplySuccessor;
    }
}

int parse_tlsa_reply (const unsigned char *abuf, int alen, TLSA_Reply** pTlsaReply) {
    unsigned int qdcount, ancount, i;
    const unsigned char *aptr, *vptr;
    int status, rr_type, rr_class, rr_len;
    long len;
    char *hostname = NULL, *rr_name = NULL;
    TLSA_Reply *tlsaReply = nullptr;
    TLSA_Reply *currTlsaReply = nullptr;

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
        if (rr_class == C_IN && rr_type == T_TLSA) {
            vptr = aptr;
            if (tlsaReply == nullptr) {
                tlsaReply = new TLSA_Reply;
                currTlsaReply = tlsaReply;
            } else {
                currTlsaReply->tlsaReplyNext = new TLSA_Reply;
                currTlsaReply = currTlsaReply->tlsaReplyNext;
            }
            currTlsaReply->hostname = rr_name;
            LOG_DEBUG("Hostname " << rr_name);
            currTlsaReply->certificate_usage = vptr[0];
            LOG_DEBUG("Certificate Usage " << currTlsaReply->certificate_usage);
            vptr += sizeof(unsigned char);
            currTlsaReply->selector = (uint8_t) vptr[0];
            LOG_DEBUG("Selector " << currTlsaReply->selector);
            vptr += sizeof(unsigned char);
            currTlsaReply->matching_type = (uint8_t) vptr[0];
            LOG_DEBUG("Matching Type " << currTlsaReply->matching_type);
            vptr += sizeof(unsigned char);
            // Certificate Association Data length = rr_len - Certificate Usage - Selector - Matching Type
            size_t certificate_association_data_size = rr_len - (3*sizeof(unsigned char));
            currTlsaReply->certificate_association_data.assign(vptr, vptr + certificate_association_data_size);
            LOG_DEBUG("Certificate Association Data " << currTlsaReply->certificate_association_data.data())
            vptr += certificate_association_data_size;
        }
        /* Don't lose memory in the next iteration */
        ares_free_string(rr_name);
        rr_name = NULL;

        /* Move on to the next record */
        aptr += rr_len;
    }
    ares_free_string(hostname);
    hostname = NULL;
    *pTlsaReply = tlsaReply;
    return ARES_SUCCESS;
}