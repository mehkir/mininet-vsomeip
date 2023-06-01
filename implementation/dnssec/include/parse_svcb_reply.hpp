//
// Created by mehkir on 21.09.22.
//

#ifndef VSOMEIP_PARSE_SVCB_REPLY_HPP
#define VSOMEIP_PARSE_SVCB_REPLY_HPP

#include <iostream>

#define T_SVCB 64

struct svcb_key {
    svcb_key* svcb_key_next_ = nullptr;
    uint16_t key_num_ = 0;
    std::string key_val_ = "";
};

enum SVCB_PARAM_TYPE {
    MANDATORY,
    ALPN,
    NO_DEFAULT_ALPN,
    PORT,
    IPV4HINT,
    ECH,
    IPV6HINT,
    LOWERKEY = 65280,
    UPPERKEY = 65534
};

struct svcb_reply {
    svcb_reply* svcb_reply_next_ = nullptr;
    std::string hostname_ = "";
    u_int16_t svc_priority_ = 0;
    std::string target_ = "";
    uint16_t port_ = 0;
    uint32_t ipv4_address_numerical_ = 0;
    std::string ipv4_address_string_ = "";
    svcb_key* svcb_key_ = nullptr;
    std::string get_svcb_key(uint16_t _key_num) {
        svcb_key* svcb_key = this->svcb_key_;
        while (svcb_key != nullptr && svcb_key->key_num_ != _key_num) {
            svcb_key = svcb_key->svcb_key_next_;
        }
        return svcb_key != nullptr ? svcb_key->key_val_ : "nullptr";
    }
    friend std::ostream& operator<<(std::ostream& _os, const svcb_reply& _svcb_reply)
    {
        _os << "SVCB Reply: " << _svcb_reply.hostname_ << "\n"
           << "\tSvcPriority\t" << _svcb_reply.svc_priority_ << "\n"
           << "\tTargetName\t" << _svcb_reply.target_ << "\n"
           << "\tPort\t\t" << _svcb_reply.port_ << "\n"
           << "\tipv4hint\t" << _svcb_reply.ipv4_address_string_ << "\n";
        const svcb_key* svcb_key = _svcb_reply.svcb_key_;
        while (svcb_key != nullptr) {
            _os << "\tkey" << svcb_key->key_num_ << "\t" << svcb_key->key_val_;
            if ((svcb_key = svcb_key->svcb_key_next_) != nullptr)
                _os << "\n";
        }
        return _os;
    }
};

void delete_svcb_reply(svcb_reply* _svcb_reply);
int parse_svcb_reply (const unsigned char* _abuf, int _alen, svcb_reply** _p_svcb_reply);

#endif //VSOMEIP_PARSE_SVCB_REPLY_HPP
