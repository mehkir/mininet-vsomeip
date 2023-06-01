//
// Created by mehkir on 21.09.22.
//

#ifndef VSOMEIP_DNS_RESOLVER_HPP
#define VSOMEIP_DNS_RESOLVER_HPP

#define DEFAULT_SERVER 0xAC110002 // 0xAC=172 0x11=17 0x00=0 0x02=2 -> 172.17.0.2

#include <mutex>
#include <thread>
#include <ares.h>
#include <stdexcept>
#include <iostream>
#include <condition_variable>
#include <deque>

struct dns_request {
    const char* name_ = "";
    int dnsclass_ = 0;
    int type_ = 0;
    ares_callback callback_ = nullptr;
    void* arg_ = nullptr;
};

class dns_resolver {

public:
    static dns_resolver* get_instance();
    int initialize(in_addr_t _address=DEFAULT_SERVER);
    void cleanup();
    void resolve(const char* _name, int _dnsclass, int _type, ares_callback _callback, void* _arg);
protected:
private:
    dns_resolver();
    ~dns_resolver();
    const int STARTED = 0;
    const int STOPPED = 1;
    int state_ = STOPPED;
    bool initialized_ = false;
    ares_channel channel_;
    std::thread process_thread_;
    std::condition_variable condition_variable_;
    static std::mutex mutex_;
    static dns_resolver* instance_;
    void process();
    int change_dns_server(ares_channel& _channel, in_addr_t _address);
    std::deque<dns_request> dns_requests_;
};

#endif //VSOMEIP_DNS_RESOLVER_HPP
