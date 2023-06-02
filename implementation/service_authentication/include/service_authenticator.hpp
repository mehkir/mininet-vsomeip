#ifndef VSOMEIP_SERVICE_AUTHENTICATOR_HPP
#define VSOMEIP_SERVICE_AUTHENTICATOR_HPP

#include <mutex>

class service_authenticator
{
private:
    service_authenticator();
    ~service_authenticator();
    static std::mutex mutex_;
    static service_authenticator* instance_;
public:
    static service_authenticator* get_instance();
};

#endif /* VSOMEIP_SERVICE_AUTHENTICATOR_HPP */