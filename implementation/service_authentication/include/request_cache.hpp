#ifndef VSOMEIP_REQUEST_CACHE_HPP
#define VSOMEIP_REQUEST_CACHE_HPP

#include <map>
#include <tuple>
#include <boost/asio/ip/address_v4.hpp>
#include "vsomeip/primitive_types.hpp"

#define IP_ADDRESS  boost::asio::ip::address_v4

struct challenger_data {
    int random_nonce;
    unsigned char* certificate;
};

class request_cache
{
private:
    std::map<std::tuple<IP_ADDRESS, std::tuple<vsomeip_v3::service_t, vsomeip_v3::instance_t>>, challenger_data> request_cache_map;
public:
    request_cache(/* args */);
    ~request_cache();
};

#endif /* VSOMEIP_REQUEST_CACHE_HPP */