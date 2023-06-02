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

void delete_tlsa_reply(tlsa_reply* _tlsa_reply) {
    tlsa_reply* tlsa_reply_current = _tlsa_reply;
    tlsa_reply* tlsa_reply_successor = nullptr;
    while (tlsa_reply_current != nullptr) {
        LOG_DEBUG("Delete TLSA reply: " << tlsa_reply_current->hostname_)
        tlsa_reply_successor = tlsa_reply_current->tlsa_reply_next_;
        delete (tlsa_reply_current);
        tlsa_reply_current = tlsa_reply_successor;
    }
}

int parse_tlsa_reply (const unsigned char* _abuf, int _alen, tlsa_reply** _p_tlsa_reply) {
    unsigned int qdcount, ancount, i;
    const unsigned char *aptr, *vptr;
    int status, rr_type, rr_class, rr_len;
    long len;
    char *hostname = NULL, *rr_name = NULL;
    tlsa_reply *tlsareply = nullptr;
    tlsa_reply *curr_tlsa_reply = nullptr;

    /* Give up if abuf doesn't have room for a header. */
    if (_alen < HFIXEDSZ)
        return ARES_EBADRESP;

    /* Fetch the question and answer count from the header. */
    qdcount = DNS_HEADER_QDCOUNT(_abuf);
    ancount = DNS_HEADER_ANCOUNT(_abuf);
    if (qdcount != 1)
        return ARES_EBADRESP;
    if (ancount == 0)
        return ARES_ENODATA;

    /* Expand the name from the question, and skip past the question. */
    aptr = _abuf + HFIXEDSZ;
    status = ares_expand_name(aptr, _abuf, _alen, &hostname, &len);
    if (status != ARES_SUCCESS)
        return status;

    if (aptr + len + QFIXEDSZ > _abuf + _alen) {
        ares_free_string(hostname);
        return ARES_EBADRESP;
    }
    aptr += len + QFIXEDSZ;

    /* Examine each answer resource record (RR) in turn. */
    for (i = 0; i < ancount; i++) {
        /* Decode the RR up to the data field. */
        status = ares_expand_name(aptr, _abuf, _alen, &rr_name, &len);
        if (status != ARES_SUCCESS) {
            break;
        }
        aptr += len;
        if (aptr + RRFIXEDSZ > _abuf + _alen) {
            status = ARES_EBADRESP;
            break;
        }
        rr_type = DNS_RR_TYPE (aptr);
        rr_class = DNS_RR_CLASS (aptr);
        rr_len = DNS_RR_LEN (aptr);
        aptr += RRFIXEDSZ;
        if (aptr + rr_len > _abuf + _alen) {
            status = ARES_EBADRESP;
            break;
        }
        if (rr_class == C_IN && rr_type == T_TLSA) {
            vptr = aptr;
            if (tlsareply == nullptr) {
                tlsareply = new tlsa_reply;
                curr_tlsa_reply = tlsareply;
            } else {
                curr_tlsa_reply->tlsa_reply_next_ = new tlsa_reply;
                curr_tlsa_reply = curr_tlsa_reply->tlsa_reply_next_;
            }
            curr_tlsa_reply->hostname_ = rr_name;
            LOG_DEBUG("Hostname " << rr_name);
            curr_tlsa_reply->certificate_usage_ = vptr[0];
            LOG_DEBUG("Certificate Usage " << curr_tlsa_reply->certificate_usage_);
            vptr += sizeof(unsigned char);
            curr_tlsa_reply->selector_ = (uint8_t) vptr[0];
            LOG_DEBUG("Selector " << curr_tlsa_reply->selector_);
            vptr += sizeof(unsigned char);
            curr_tlsa_reply->matching_type_ = (uint8_t) vptr[0];
            LOG_DEBUG("Matching Type " << curr_tlsa_reply->matching_type_);
            vptr += sizeof(unsigned char);
            // Certificate Association Data length = rr_len - Certificate Usage - Selector - Matching Type
            size_t certificate_association_data_size = rr_len - (3*sizeof(unsigned char));
            curr_tlsa_reply->certificate_association_data_.assign(vptr, vptr + certificate_association_data_size);
            LOG_DEBUG("Certificate Association Data " << curr_tlsa_reply->certificate_association_data_.data())
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
    *_p_tlsa_reply = tlsareply;
    return ARES_SUCCESS;
}