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

struct TLSA_Reply {
    TLSA_Reply* tlsaReplyNext = nullptr;
    std::string hostname = "";
    uint8_t certificate_usage = 0;
    uint8_t selector = 0;
    uint8_t matching_type = 0;
    std::vector<unsigned char> certificate_association_data;
    friend std::ostream& operator<<(std::ostream& os, const TLSA_Reply& tlsaReply)
    {
        std::stringstream ss;
        int linebreakcount = 0;
        for (auto it = tlsaReply.certificate_association_data.begin(); it != tlsaReply.certificate_association_data.end(); it++) {
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
        os << "TLSA Reply: " << tlsaReply.hostname << "\n"
           << "\tCertificate Usage\t" << (unsigned int) tlsaReply.certificate_usage << "\n"
           << "\tSelector\t\t\t" << (unsigned int) tlsaReply.selector << "\n"
           << "\tMatching Type\t\t" << (unsigned int) tlsaReply.matching_type << "\n"
           << "\tCertificate Association Data" << "\n"
           << ss.str() << "\n";
        return os;
    }
};

void delete_tlsa_reply(TLSA_Reply* tlsaReply);
int parse_tlsa_reply (const unsigned char *abuf, int alen, TLSA_Reply** pTlsaReply);

#endif //VSOMEIP_PARSE_TLSA_REPLY_HPP
