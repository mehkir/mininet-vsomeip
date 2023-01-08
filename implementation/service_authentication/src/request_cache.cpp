#include "../include/request_cache.hpp"
#include <iostream>

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

void request_cache::addRequestNonce(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                               vsomeip_v3::instance_t instanceId, unsigned int nonce) {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    auto keyTuple = makeKeyTuple(ipAddress, serviceId, instanceId);
    if (!request_map.count(keyTuple)) {
        request_map[keyTuple] = challenge_response_data();
    }
    request_map[keyTuple].random_nonce = nonce;
}

void request_cache::addRequestCertificate(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char> certificate_data) {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    auto keyTuple = makeKeyTuple(ipAddress, serviceId, instanceId);
    if (!request_map.count(keyTuple)) {
        request_map[keyTuple] = challenge_response_data();
    }
    request_map[keyTuple].certificate_data = certificate_data;
}

challenge_response_data request_cache::getRequest(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId) {
    auto keyTuple = makeKeyTuple(ipAddress, serviceId, instanceId);
    return request_map[keyTuple];
}

void request_cache::removeRequest(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                vsomeip_v3::instance_t instanceId) {
    auto keyTuple = makeKeyTuple(ipAddress, serviceId, instanceId);
    request_map.erase(keyTuple);
}

std::tuple<boost::asio::ip::address_v4, vsomeip_v3::service_t,
                    vsomeip_v3::instance_t> request_cache::makeKeyTuple(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId) {
    return std::make_tuple(ipAddress, serviceId, instanceId);
}