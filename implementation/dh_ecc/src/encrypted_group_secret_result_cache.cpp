#include "../include/encrypted_group_secret_result_cache.hpp"

namespace vsomeip_v3 {

encrypted_group_secret_result_cache::encrypted_group_secret_result_cache() {
}

encrypted_group_secret_result_cache::~encrypted_group_secret_result_cache() {
}

void encrypted_group_secret_result_cache::add_encrypted_group_secret_result(boost::asio::ip::address_v4 _ipv4_address, service_t _service, instance_t _instance, major_version_t _major, encrypted_group_secret_result _encrypted_group_secret_result) {
    std::lock_guard<std::mutex> lockguard(mutex_);
    auto key_tuple = make_key_tuple(_ipv4_address, _service, _instance, _major);
    encrypted_group_secret_result_cache_map_[key_tuple] = _encrypted_group_secret_result;
}

encrypted_group_secret_result encrypted_group_secret_result_cache::get_encrypted_group_secret_result(boost::asio::ip::address_v4 _ipv4_address, service_t _service, instance_t _instance, major_version_t _major) {
    std::lock_guard<std::mutex> lockguard(mutex_);
    auto key_tuple = make_key_tuple(_ipv4_address, _service, _instance, _major);
    return encrypted_group_secret_result_cache_map_[key_tuple];
}

void encrypted_group_secret_result_cache::remove_encrypted_group_secret_result(boost::asio::ip::address_v4 _ipv4_address, service_t _service, instance_t _instance, major_version_t _major) {
    std::lock_guard<std::mutex> lockguard(mutex_);
    auto key_tuple = make_key_tuple(_ipv4_address, _service, _instance, _major);
    encrypted_group_secret_result_cache_map_.erase(key_tuple);
}

std::tuple<boost::asio::ip::address_v4, service_t, instance_t, major_version_t> encrypted_group_secret_result_cache::make_key_tuple(boost::asio::ip::address_v4 _ipv4_address, service_t _service, instance_t _instance, major_version_t _major) {
    return std::make_tuple(_ipv4_address, _service, _instance, _major);
}

}; /* end namespace vsomeip_v3 */