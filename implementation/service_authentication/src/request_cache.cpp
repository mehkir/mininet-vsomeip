#include "../include/request_cache.hpp"

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
    if (!request_map.count({ipAddress, serviceId, instanceId})) {
        request_map[{ipAddress, serviceId, instanceId}] = challenger_data();
    }
    request_map[{ipAddress, serviceId, instanceId}].random_nonce = nonce;
}

void request_cache::addRequestCertificate(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId, std::vector<unsigned char> certificate_data) {
    if (!request_map.count({ipAddress, serviceId, instanceId})) {
        request_map[{ipAddress, serviceId, instanceId}] = challenger_data();
    }
    std::vector<unsigned char>& data = request_map[{ipAddress, serviceId, instanceId}].certificate_data;
    data.insert(data.end(), certificate_data.begin(), certificate_data.end());
}

challenger_data request_cache::getRequest(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                    vsomeip_v3::instance_t instanceId) {
    return request_map[{ipAddress, serviceId, instanceId}];
}

void request_cache::removeRequest(boost::asio::ip::address_v4 ipAddress, vsomeip_v3::service_t serviceId,
                vsomeip_v3::instance_t instanceId) {
    request_map.erase({ipAddress, serviceId, instanceId});
}