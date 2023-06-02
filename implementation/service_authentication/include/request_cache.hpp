#ifndef VSOMEIP_V3_REQUEST_CACHE_HPP
#define VSOMEIP_V3_REQUEST_CACHE_HPP

#include <mutex>
#include <map>
#include <tuple>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>
#include "vsomeip/primitive_types.hpp"

namespace vsomeip_v3 {

struct challenge_response_data {
    std::vector<unsigned char> nonce_;
    std::vector<unsigned char> certificate_data_;
};

class request_cache {
private:
    request_cache();
    ~request_cache();
    static std::mutex mutex_;
    static request_cache* instance_;
    std::map<std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t, vsomeip_v3::instance_t>, challenge_response_data> request_map_;
public:
    static request_cache* get_instance();

    request_cache(request_cache const&) = delete;
    request_cache(request_cache&&) = delete;
    request_cache& operator=(request_cache &) = delete;
    request_cache& operator=(request_cache &&) = delete;

    void add_request_nonce(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce);
    bool has_nonce_and_remove(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce);
    void add_request_certificate(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance, std::vector<unsigned char> _certificate_data);
    std::vector<unsigned char> get_request_nonce(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance);
    std::vector<unsigned char> get_request_certificate(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance);
    void remove_request(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance);
    std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t,
                    vsomeip_v3::instance_t> make_key_tupe(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance);
};
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_REQUEST_CACHE_HPP */