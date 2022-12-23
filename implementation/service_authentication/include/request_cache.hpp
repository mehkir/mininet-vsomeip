#ifndef VSOMEIP_REQUEST_CACHE_HPP
#define VSOMEIP_REQUEST_CACHE_HPP

#include <mutex>
#include <map>
#include <tuple>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>
#include "vsomeip/primitive_types.hpp"

struct challenge_response_data {
    unsigned int random_nonce;
    std::vector<unsigned char> certificate_data;
};

class request_cache
{
private:
    request_cache();
    ~request_cache();
    static std::mutex mutex_;
    static request_cache* instance;
    std::map<std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t, vsomeip_v3::instance_t>, challenge_response_data> request_map;
public:
    static request_cache* getInstance();
    void addRequestNonce(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, unsigned nonce);
    void addRequestCertificate(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char> certificate_data);
    challenge_response_data getRequest(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId);
    void removeRequest(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId);
};

#endif /* VSOMEIP_REQUEST_CACHE_HPP */