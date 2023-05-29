#include "../include/service_authenticator.hpp"

std::mutex service_authenticator::mutex_;
service_authenticator* service_authenticator::instance;

service_authenticator::service_authenticator() {
}

service_authenticator::~service_authenticator() {
}

service_authenticator* service_authenticator::get_instance() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if(instance == nullptr) {
        instance = new service_authenticator();
    }
    return instance;
}