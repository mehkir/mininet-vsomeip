#ifndef VSOMEIP_REQUEST_CACHE_HPP
#define VSOMEIP_REQUEST_CACHE_HPP

#include <mutex>
#include <map>
#include <tuple>
#include <vector>
#include <set>
#include <boost/asio/ip/address_v4.hpp>
#include "vsomeip/primitive_types.hpp"

namespace vsomeip_v3 {

struct challenge_response_data {
    std::set<std::vector<unsigned char>> random_nonces;
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

    request_cache(request_cache const&) = delete;
    request_cache(request_cache&&) = delete;
    request_cache& operator=(request_cache &) = delete;
    request_cache& operator=(request_cache &&) = delete;

    void add_request_nonce(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char> nonce);
    bool has_nonce_and_remove(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char> nonce);
    bool get_nonce_and_remove(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char>& nonce);
    void add_request_certificate(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char> certificate_data);
    std::vector<unsigned char> get_request_certificate(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId);
    void remove_request(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId);
    std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t,
                    vsomeip_v3::instance_t> make_key_tupe(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId);
};
} /* end namespace vsomeip_v3*/
#endif /* VSOMEIP_REQUEST_CACHE_HPP */