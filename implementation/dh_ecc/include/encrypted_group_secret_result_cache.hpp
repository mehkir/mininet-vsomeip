#ifndef VSOMEIP_ENCRYPTED_GROUP_SECRET_RESULT_CACHE_HPP
#define VSOMEIP_ENCRYPTED_GROUP_SECRET_RESULT_CACHE_HPP

#include "dh_ecc.hpp"
#include "vsomeip/primitive_types.hpp"
#include <mutex>
#include <map>
#include <tuple>
#include <boost/asio/ip/address_v4.hpp>


namespace vsomeip_v3 {
    class encrypted_group_secret_result_cache {
    private:
        std::mutex mutex_;
        std::map<std::tuple<boost::asio::ip::address_v4, service_t, instance_t, major_version_t>, encrypted_group_secret_result> encrypted_group_secret_result_cache_map_;
        std::tuple<boost::asio::ip::address_v4, service_t, instance_t, major_version_t> make_key_tuple(boost::asio::ip::address_v4 _ipv4_address, service_t _service, instance_t _instance, major_version_t _major);
    public:
        encrypted_group_secret_result_cache();
        ~encrypted_group_secret_result_cache();
        void add_encrypted_group_secret_result(boost::asio::ip::address_v4 _ipv4_address, service_t _service, instance_t _instance, major_version_t _major, encrypted_group_secret_result _encrypted_group_secret_result);
        encrypted_group_secret_result get_encrypted_group_secret_result(boost::asio::ip::address_v4 _ipv4_address, service_t _service, instance_t _instance, major_version_t _major);
        void remove_encrypted_group_secret_result(boost::asio::ip::address_v4 _ipv4_address, service_t _service, instance_t _instance, major_version_t _major);
    };
    
} /* end namespace vsomeip_v3 */

#endif /* VSOMEIP_ENCRYPTED_GROUP_SECRET_RESULT_CACHE_HPP */