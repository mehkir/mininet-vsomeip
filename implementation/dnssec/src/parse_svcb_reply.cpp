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

void delete_svcb_reply(svcb_reply* _svcb_reply) {
    svcb_reply* svcb_reply_current = _svcb_reply;
    svcb_reply* svcb_reply_successor = nullptr;
    while (svcb_reply_current != nullptr) {
        LOG_DEBUG("Delete SVCB reply: " << svcb_reply_current->hostname_)
        svcb_key *svcb_key_current = svcb_reply_current->svcb_key_;
        svcb_key *svcb_key_successor = nullptr;
        while (svcb_key_current != nullptr) {
            svcb_key_successor = svcb_key_current->svcb_key_next_;
            LOG_DEBUG("\tDelete key\t" << svcb_key_current->key_num_)
            delete(svcb_key_current);
            svcb_key_current = svcb_key_successor;
        }
        svcb_reply_successor = svcb_reply_current->svcb_reply_next_;
        delete (svcb_reply_current);
        svcb_reply_current = svcb_reply_successor;
    }
}

int parse_svcb_reply (const unsigned char* _abuf, int _alen, svcb_reply** _p_svcb_reply) {
    unsigned int qdcount, ancount, i;
    const unsigned char *aptr, *vptr;
    int status, rr_type, rr_class, rr_len;
    long len;
    char *hostname = NULL, *rr_name = NULL, *target_name = NULL;
    svcb_reply *svcbreply = nullptr;
    svcb_reply *curr_svcb_reply = nullptr;

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
        if (rr_class == C_IN && rr_type == T_SVCB) {
            vptr = aptr;
            svcb_key* svcbkey = nullptr;
            if (svcbreply == nullptr) {
                svcbreply = new svcb_reply;
                curr_svcb_reply = svcbreply;
            } else {
                curr_svcb_reply->svcb_reply_next_ = new svcb_reply;
                curr_svcb_reply = curr_svcb_reply->svcb_reply_next_;
            }
            curr_svcb_reply->hostname_ = hostname;
            LOG_DEBUG("Hostname " << hostname);
            curr_svcb_reply->svc_priority_ = DNS__16BIT(vptr);
            LOG_DEBUG("SvcPriority " << curr_svcb_reply->svc_priority_)
            vptr += sizeof(unsigned short);
            status = ares_expand_name(vptr, _abuf, _alen, &target_name, &len);
            if (status != ARES_SUCCESS) {
                break;
            }
            LOG_DEBUG("Target " << curr_svcb_reply->target_)
            curr_svcb_reply->target_ = std::string(target_name) == "" ? "." : std::string(target_name);
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
                        curr_svcb_reply->port_ = DNS__16BIT(vptr);
                        vptr += length * sizeof(unsigned char);
                    }
                        break;
                    case SVCB_PARAM_TYPE::IPV4HINT: {
                        LOG_DEBUG("ipv4hint")
                        vptr += sizeof(unsigned short);
                        int length = DNS__16BIT(vptr);
                        LOG_DEBUG("Length " << length)
                        vptr += sizeof(unsigned short);                        
                        curr_svcb_reply->ipv4_address_numerical_ = DNS__32BIT(vptr);
                        LOG_DEBUG("IP address (numerical) " << curr_svcb_reply->ipv4_address_numerical_)
                        std::stringstream sstream;
                        sstream << ((curr_svcb_reply->ipv4_address_numerical_ & 0xff000000) >> 24) << "."
                                << ((curr_svcb_reply->ipv4_address_numerical_ & 0x00ff0000) >> 16) << "."
                                << ((curr_svcb_reply->ipv4_address_numerical_ & 0x0000ff00) >> 8) << "."
                                << (curr_svcb_reply->ipv4_address_numerical_ & 0x000000ff);
                        curr_svcb_reply->ipv4_address_string_ = sstream.str();
                        LOG_DEBUG("IP address (string) " << curr_svcb_reply->ipv4_address_string_)
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
                            uint16_t key_number = DNS__16BIT(vptr);
                            LOG_DEBUG("Key number " << key_number)
                            vptr += sizeof(unsigned short);
                            int length = DNS__16BIT(vptr);
                            LOG_DEBUG("Length " << length)
                            vptr += sizeof(unsigned short);
                            std::string key_value = std::string((char*) vptr,length);
                            LOG_DEBUG("Key value " << key_value)
                            vptr += length * sizeof(unsigned char);
                            if (curr_svcb_reply->svcb_key_ == nullptr) {
                                curr_svcb_reply->svcb_key_ = new svcb_key;
                                svcbkey = curr_svcb_reply->svcb_key_;
                            } else {
                                svcbkey->svcb_key_next_ = new svcb_key;
                                svcbkey = svcbkey->svcb_key_next_;
                            }
                            svcbkey->key_num_ = key_number;
                            svcbkey->key_val_ = key_value;
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
    *_p_svcb_reply = svcbreply;
    return ARES_SUCCESS;
}