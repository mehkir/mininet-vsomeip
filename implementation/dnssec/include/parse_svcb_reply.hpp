//
// Created by mehkir on 21.09.22.
//

#ifndef VSOMEIP_PARSE_SVCB_REPLY_HPP
#define VSOMEIP_PARSE_SVCB_REPLY_HPP

#include <iostream>

#define T_SVCB 64

struct SVCB_Key {
    SVCB_Key* svcbKeyNext = nullptr;
    uint16_t keyNum = 0;
    std::string keyVal = "";
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

struct SVCB_Reply {
    SVCB_Reply* svcbReplyNext = nullptr;
    std::string hostname = "";
    u_int16_t svcPriority = 0;
    std::string target = "";
    uint16_t port = 0;
    uint32_t ipv4AddressNumerical = 0;
    std::string ipv4AddressString = "";
    SVCB_Key* svcbKey = nullptr;
    std::string getSVCBKey(uint16_t keyNum) {
        SVCB_Key* svcbKey = this->svcbKey;
        while (svcbKey != nullptr && svcbKey->keyNum != keyNum) {
            svcbKey = svcbKey->svcbKeyNext;
        }
        return svcbKey != nullptr ? svcbKey->keyVal : "nullptr";
    }
    friend std::ostream& operator<<(std::ostream& os, const SVCB_Reply& svcbReply)
    {
        os << "SVCB Reply: " << svcbReply.hostname << "\n"
           << "\tSvcPriority\t" << svcbReply.svcPriority << "\n"
           << "\tTargetName\t" << svcbReply.target << "\n"
           << "\tPort\t\t" << svcbReply.port << "\n"
           << "\tipv4hint\t" << svcbReply.ipv4AddressString << "\n";
        const SVCB_Key* svcbKey = svcbReply.svcbKey;
        while (svcbKey != nullptr) {
            os << "\tkey" << svcbKey->keyNum << "\t" << svcbKey->keyVal;
            if ((svcbKey = svcbKey->svcbKeyNext) != nullptr)
                os << "\n";
        }
        return os;
    }
};

void delete_svcb_reply(SVCB_Reply* svcbReply);
int parse_svcb_reply (const unsigned char *abuf, int alen, SVCB_Reply** pSvcbReply);

#endif //VSOMEIP_PARSE_SVCB_REPLY_HPP
