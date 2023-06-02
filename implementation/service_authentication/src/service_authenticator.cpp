#include "../include/service_authenticator.hpp"

std::mutex service_authenticator::mutex_;
service_authenticator* service_authenticator::instance_;

service_authenticator::service_authenticator() {
}

service_authenticator::~service_authenticator() {
}

service_authenticator* service_authenticator::get_instance() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if(instance_ == nullptr) {
        instance_ = new service_authenticator();
    }
    return instance_;
}