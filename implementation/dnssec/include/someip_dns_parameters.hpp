#ifndef VSOMEIP_V3_SOMEIP_DNS_PARAMETERS_H
#define VSOMEIP_V3_SOMEIP_DNS_PARAMETERS_H

#include "vsomeip/primitive_types.hpp"
#include "../../service_authentication/include/crypto_operator.hpp"
#include <vector>
#include <functional>
#include <boost/asio/ip/address_v4.hpp>

#define DNS_SERVER_IP 0xAC110005
#define INSTANCE 65280
#define MAJOR_VERSION 65281
#define MINOR_VERSION 65282
#define L4PROTOCOL 65283

#define ATTRLEAFBRANCH "_someip."
#define PARENTDOMAIN "service."

//struct service_data_and_callbacks;
typedef std::function<void(boost::asio::ip::address_v4, service_t, instance_t, std::vector<unsigned char>)> request_cache_callback;
typedef std::function<void(void*, std::string)> request_tlsa_record_callback;
typedef std::function<void(std::string)> record_timestamp_callback;
typedef std::function<std::vector<CryptoPP::byte>(const std::vector<CryptoPP::byte>)> convert_DER_to_PEM_callback;
struct service_data_and_cbs {
    service_t service;
    instance_t instance;
    major_version_t major;
    minor_version_t minor;
    boost::asio::ip::address_v4 ip_address;
    request_cache_callback request_cache_callback_;
    request_tlsa_record_callback request_tlsa_record_callback_;
    record_timestamp_callback record_timestamp_callback_;
    convert_DER_to_PEM_callback convert_DER_to_PEM_callback_;
};

#endif /* VSOMEIP_V3_SOMEIP_DNS_PARAMETERS_H */