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
    challenge_response_cache();
    ~challenge_response_cache();
    static std::mutex mutex_;
    static challenge_response_cache* instance_;
    std::map<std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t, vsomeip_v3::instance_t>, challenge_response_data> challenge_response_map_;
public:
    static challenge_response_cache* get_instance();

    challenge_response_cache(challenge_response_cache const&) = delete;
    challenge_response_cache(challenge_response_cache&&) = delete;
    challenge_response_cache& operator=(challenge_response_cache &) = delete;
    challenge_response_cache& operator=(challenge_response_cache &&) = delete;

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
};
} /* end namespace vsomeip_v3 */
#endif /* VSOMEIP_V3_CHALLENGE_RESPONSE_CACHE_HPP */