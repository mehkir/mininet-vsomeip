#include "../include/request_cache.hpp"
#include <iostream>

namespace vsomeip_v3 {

std::mutex request_cache::mutex_;
request_cache* request_cache::instance;

request_cache::request_cache()
{
}

request_cache::~request_cache()
{
}

request_cache* request_cache::getInstance() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if(instance == nullptr) {
        instance = new request_cache();
    }
    return instance;
}

void request_cache::add_request_nonce(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                               vsomeip_v3::instance_t instanceId, std::vector<unsigned char> nonce) {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    auto keyTuple = make_key_tupe(ipAddress, serviceId, instanceId);
    if (!request_map.count(keyTuple)) {
        request_map[keyTuple] = challenge_response_data();
    }
    request_map[keyTuple].random_nonces.insert(nonce);
}

bool request_cache::has_nonce_and_remove(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char> nonce) {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    auto keyTuple = make_key_tupe(ipAddress, serviceId, instanceId);
    return request_map.count(keyTuple) && request_map[keyTuple].random_nonces.count(nonce) && request_map[keyTuple].random_nonces.erase(nonce);
}

bool request_cache::get_nonce_and_remove(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char> &nonce) {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    bool result = false;
    auto keyTuple = make_key_tupe(ipAddress, serviceId, instanceId);
    result = request_map.count(keyTuple) && request_map[keyTuple].random_nonces.size();
    if (result)  {
        nonce = *(request_map[keyTuple].random_nonces.begin());
        request_map[keyTuple].random_nonces.erase(nonce);
    }
    return result;
}

void request_cache::add_request_certificate(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char> certificate_data) {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    auto keyTuple = make_key_tupe(ipAddress, serviceId, instanceId);
    if (!request_map.count(keyTuple)) {
        request_map[keyTuple] = challenge_response_data();
    }
    request_map[keyTuple].certificate_data = certificate_data;
}

std::vector<unsigned char> request_cache::get_request_certificate(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId) {
    auto keyTuple = make_key_tupe(ipAddress, serviceId, instanceId);
    return request_map[keyTuple].certificate_data;
}

void request_cache::remove_request(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                vsomeip_v3::instance_t instanceId) {
    auto keyTuple = make_key_tupe(ipAddress, serviceId, instanceId);
    request_map.erase(keyTuple);
}

std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t,
                    vsomeip_v3::instance_t> request_cache::make_key_tupe(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId) {
    return std::make_tuple(ipAddress, serviceId, instanceId);
}
} /* end namespace vsomeip_v3 */