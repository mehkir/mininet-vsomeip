#ifndef VSOMEIP_V3_SOMEIP_DNS_PARAMETERS_H
#define VSOMEIP_V3_SOMEIP_DNS_PARAMETERS_H

#include <vsomeip/constants.hpp>
#include "../../service_authentication/include/crypto_operator.hpp"
#include <vector>
#include <functional>
#include <boost/asio/ip/address_v4.hpp>

#define DNS_SERVER_IP 0xAC120004
#define INSTANCE 65280
#define MAJOR_VERSION 65281
#define MINOR_VERSION 65282
#define L4PROTOCOL 65283
#define CLIENT_PORTS 65284

#define ATTRLEAFBRANCH "_someip."
#define SERVICE_PARENTDOMAIN "service."
#define CLIENT_PARENTDOMAIN "client."

namespace vsomeip_v3 {
    //struct service_data_and_callbacks
    typedef std::function<void(service_t, instance_t, major_version_t, minor_version_t, int, const boost::asio::ip::address_v4, uint16_t)> add_service_svcb_entry_cache_callback;
    typedef std::function<void(service_t, instance_t, major_version_t, minor_version_t)> verify_service_info_callback;
    typedef std::function<void(boost::asio::ip::address_v4, service_t, instance_t, std::vector<unsigned char>)> add_publisher_certificate_callback;
    typedef std::function<void(void*, std::string)> request_service_tlsa_record_callback;
    typedef std::function<void(boost::asio::ip::address_v4 _sender_ip_address, service_t _service, instance_t _instance)> verify_publisher_signature_callback;
    typedef std::function<void(std::string)> record_timestamp_callback;
    typedef std::function<std::vector<CryptoPP::byte>(const std::vector<CryptoPP::byte>)> convert_der_to_pem_callback;
    struct service_data_and_cbs {
        service_t service_;
        instance_t instance_;
        major_version_t major_;
        minor_version_t minor_;
        boost::asio::ip::address_v4 ipv4_address_;
        add_service_svcb_entry_cache_callback add_service_svcb_entry_cache_callback_;
        verify_service_info_callback verify_service_info_callback_;
        add_publisher_certificate_callback add_publisher_certificate_callback_;
        request_service_tlsa_record_callback request_service_tlsa_record_callback_;
        verify_publisher_signature_callback verify_publisher_signature_callback_;
        record_timestamp_callback record_timestamp_callback_;
        convert_der_to_pem_callback convert_der_to_pem_callback_;
    };

    //struct client_data_and_callbacks
    typedef std::function<void(client_t, service_t, instance_t, major_version_t, minor_version_t, int, const boost::asio::ip::address_v4, uint16_t)> add_client_svcb_entry_cache_callback;
    struct client_data_and_cbs {
        client_t client_;
        service_t service_;
        instance_t instance_;
        major_version_t major_;
        boost::asio::ip::address_v4 ipv4_address_;
        add_client_svcb_entry_cache_callback add_client_svcb_entry_cache_callback_;
    };
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_SOMEIP_DNS_PARAMETERS_H */