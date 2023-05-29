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

struct DNSRequest {
    const char *name = "";
    int dnsclass = 0;
    int type = 0;
    ares_callback callback = nullptr;
    void *arg = nullptr;
};

class dns_resolver {

public:
    static dns_resolver* get_instance();
    int initialize(in_addr_t address=DEFAULT_SERVER);
    void cleanup();
    void resolve(const char *name, int dnsclass, int type, ares_callback callback, void *arg);
protected:
private:
    dns_resolver();
    ~dns_resolver();
    const int STARTED = 0;
    const int STOPPED = 1;
    int state = STOPPED;
    bool initialized = false;
    ares_channel channel;
    std::thread processThread;
    std::condition_variable conditionVariable;
    static std::mutex mutex_;
    static dns_resolver* instance;
    void process();
    int change_dns_server(ares_channel& channel, in_addr_t address);
    std::deque<DNSRequest> dns_requests_;
};

#endif //VSOMEIP_DNS_RESOLVER_HPP
