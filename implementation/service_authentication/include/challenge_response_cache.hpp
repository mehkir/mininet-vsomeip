#ifndef VSOMEIP_V3_CHALLENGE_RESPONSE_CACHE_HPP
#define VSOMEIP_V3_CHALLENGE_RESPONSE_CACHE_HPP

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

class challenge_response_cache {
private:
    std::mutex mutex_;
    std::map<std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t, vsomeip_v3::instance_t>, challenge_response_data> challenge_response_map_;
    std::map<std::tuple<vsomeip_v3::service_t, vsomeip_v3::instance_t>, std::vector<unsigned char>> offered_nonce_map_;
public:
    challenge_response_cache();
    ~challenge_response_cache();
    void add_nonce(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce);
    bool has_nonce_and_remove(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce);
    void add_certificate(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance, std::vector<unsigned char> _certificate_data);
    std::vector<unsigned char> get_nonce(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance);
    std::vector<unsigned char> get_certificate(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance);
    void remove_entry(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance);
    std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t,
                    vsomeip_v3::instance_t> make_key_tupe(boost::asio::ip::address_v4 _ipv4_address, vsomeip_v3::service_t _service,
                    vsomeip_v3::instance_t _instance);
    void set_offered_nonce(vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance, std::vector<unsigned char> _nonce);
    std::vector<unsigned char> get_offered_nonce(vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance);
    std::tuple<vsomeip_v3::service_t, vsomeip_v3::instance_t> make_key_tupe(vsomeip_v3::service_t _service, vsomeip_v3::instance_t _instance);
};
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_CHALLENGE_RESPONSE_CACHE_HPP */