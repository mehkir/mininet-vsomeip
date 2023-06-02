//
// Created by mehkir on 12.09.22.
//

#ifndef VSOMEIP_PARSE_TLSA_REPLY_HPP
#define VSOMEIP_PARSE_TLSA_REPLY_HPP

#define LINEBREAKTHRESHOLD 30

#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

enum CERTIFICATE_USAGE {
    PKIX_TA, // CA constraint, limits which CA can be used to issue certificates, must pass path validation
    PKIX_EE, // Service certificate constraint, limits which end entity certificate for a service, must pass path validation
    DANE_TA, // Trust anchor assertion, specifies trust anchor certificate, path validation only if not domain issued
    DANE_EE  // Domain issued certificate, allows certificate without involving third-party CA
};

enum SELECTOR {
    FULL_CERTIFICATE,
    SUBJECT_PUBLICKEY_INFO
};

enum MATCHING_TYPE {
    NO_HASH,
    SHA_256,
    SHA_512
};

struct tlsa_reply {
    tlsa_reply* tlsa_reply_next_ = nullptr;
    std::string hostname_ = "";
    uint8_t certificate_usage_ = 0;
    uint8_t selector_ = 0;
    uint8_t matching_type_ = 0;
    std::vector<unsigned char> certificate_association_data_;
    friend std::ostream& operator<<(std::ostream& _os, const tlsa_reply& _tlsa_reply)
    {
        std::stringstream ss;
        int linebreakcount = 0;
        for (auto it = _tlsa_reply.certificate_association_data_.begin(); it != _tlsa_reply.certificate_association_data_.end(); it++) {
            if (linebreakcount == 0) {
                ss << "\t\t\t\t\t\t\t\t\t";
            }
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)*it;
            linebreakcount++;
            if (linebreakcount == LINEBREAKTHRESHOLD) {
                ss << "\n";
                linebreakcount = 0;
            }
        }
        _os << "TLSA Reply: " << _tlsa_reply.hostname_ << "\n"
           << "\tCertificate Usage\t" << (unsigned int) _tlsa_reply.certificate_usage_ << "\n"
           << "\tSelector\t\t\t" << (unsigned int) _tlsa_reply.selector_ << "\n"
           << "\tMatching Type\t\t" << (unsigned int) _tlsa_reply.matching_type_ << "\n"
           << "\tCertificate Association Data" << "\n"
           << ss.str() << "\n";
        return _os;
    }
};

void delete_tlsa_reply(tlsa_reply* _tlsa_reply);
int parse_tlsa_reply(const unsigned char* _abuf, int _alen, tlsa_reply** _p_tlsa_reply);

#endif //VSOMEIP_PARSE_TLSA_REPLY_HPP
