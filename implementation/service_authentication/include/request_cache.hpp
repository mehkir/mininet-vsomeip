#ifndef VSOMEIP_REQUEST_CACHE_HPP
#define VSOMEIP_REQUEST_CACHE_HPP

#include <map>
#include <tuple>
#include <boost/asio/ip/address_v4.hpp>
#include "vsomeip/primitive_types.hpp"

struct challenger_data {
    int random_nonce;
    unsigned char* certificate;
};

class request_cache
{
private:
    std::map<std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t, vsomeip_v3::instance_t>, challenger_data> request_map;
public:
    request_cache(/* args */);
    ~request_cache();
    void addRequest(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, challenger_data challengerData);
    challenger_data getRequest(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId);
    void removeRequest(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId);
};

#endif /* VSOMEIP_REQUEST_CACHE_HPP */