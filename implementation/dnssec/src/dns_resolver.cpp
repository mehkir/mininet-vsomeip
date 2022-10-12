//
// Created by mehkir on 21.09.22.
//

#include "../include/dns_resolver.hpp"
#include "../include/logger.hpp"
#include <arpa/inet.h>
#include <string.h>

#define EDNSPKSZ 1280 // https://datatracker.ietf.org/doc/html/rfc6891

std::mutex dns_resolver::mutex_;
dns_resolver* dns_resolver::instance;

dns_resolver* dns_resolver::getInstance() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if(instance == nullptr) {
        instance = new dns_resolver();
    }
    return instance;
}

void dns_resolver::resolve(const char *name, int dnsclass, int type, ares_callback callback, void *arg) {
    if (!initialized)
        throw std::runtime_error("dns_resolver is not initialized, call initialize() first!");
    DNSRequest dnsRequest;
    char* url = (char*)malloc(strlen(name)+1);
    strcpy(url, name);
    dnsRequest.name = url;
    dnsRequest.dnsclass = dnsclass;
    dnsRequest.type = type;
    dnsRequest.callback = callback;
    dnsRequest.arg = arg;
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        dnsRequests.push_back(dnsRequest);
    }
    conditionVariable.notify_one();
}

int dns_resolver::changeIPv4DNSServer(ares_channel& channel, in_addr_t address) {
    ares_addr_node servers;
    servers.next = nullptr;
    servers.family = AF_INET;
    servers.addr.addr4.s_addr = htonl(address);
    return ares_set_servers(channel, &servers);
}

int dns_resolver::initialize(in_addr_t address) {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (!initialized) {
        ares_library_init(ARES_LIB_INIT_ALL);
        if (ares_init(&channel) != ARES_SUCCESS) {
            std::cout << "Channel initialization failed" << std::endl;
            return 1;
        }

        ares_options options;
        options.flags = 0;
        int optmask = 0;
        if (ares_save_options(channel, &options, &optmask) != ARES_SUCCESS) {
            std::cout << "Retrieving options failed" << std::endl;
            return 1;
        }
        ares_destroy(channel);
        options.flags |= ARES_FLAG_EDNS;
        optmask |= ARES_OPT_EDNSPSZ;
        options.ednspsz = EDNSPKSZ;
        if (ares_init_options(&channel, &options, optmask) != ARES_SUCCESS) {
            std::cout << "Initializing with options failed" << std::endl;
            return 1;
        }
        ares_destroy_options(&options);
        if (changeIPv4DNSServer(channel, address) != ARES_SUCCESS) {
            std::cout << "Setting servers failed" << std::endl;
            return 1;
        }
        state = STARTED;
        processThread = std::thread(&dns_resolver::process, this);
        initialized = true;
        LOG_DEBUG("Process Thread is initialized")
    } else {
        LOG_DEBUG("Process Thread is already initialized")
    }
    return ARES_SUCCESS;
}

void dns_resolver::process() {
    int nfds, count;
    fd_set readers, writers;
    struct timeval tv, *tvp;
    //bool waitBool;
    while (state != STOPPED) {
        LOG_DEBUG("Process thread performed unique lock")
        /*
        if (dnsRequests.empty()) {
            std::cout << "Process thread waits for data" << std::endl;
            waitBool = true;
        } else {
            waitBool = false;
        }
         */
        {
            std::unique_lock<std::mutex> uniqueLock(mutex_);
            conditionVariable.wait(uniqueLock, [this] { return !dnsRequests.empty() || state == STOPPED; });
        }
        /*
        if (waitBool)
            std::cout << "Process thread received signal. Processing ..." << std::endl;
        */
        if (state != STOPPED) {
            DNSRequest dnsRequest;
            {
                std::lock_guard<std::mutex> lockGuard(mutex_);
                dnsRequest = dnsRequests.front();
                dnsRequests.pop_front();
            }
            ares_search(channel, dnsRequest.name, dnsRequest.dnsclass, dnsRequest.type, dnsRequest.callback,
                        dnsRequest.arg);
            while (true) {
                FD_ZERO(&readers);
                FD_ZERO(&writers);
                nfds = ares_fds(channel, &readers, &writers);
                if (nfds == 0)
                    break;
                tvp = ares_timeout(channel, NULL, &tv);
                count = select(nfds, &readers, &writers, NULL, tvp);
                ares_process(channel, &readers, &writers);
            }
            free(const_cast<char *>(dnsRequest.name));

        } else {
            LOG_DEBUG("Process thread is about to exit")
        }
    }
    LOG_DEBUG("Process thread terminated")
}

dns_resolver::dns_resolver() {
}

dns_resolver::~dns_resolver() {
}

void dns_resolver::cleanup() {
    if (initialized) {
        LOG_DEBUG("Cleanup begins")
        state = STOPPED;
        conditionVariable.notify_one();
        processThread.join();
        ares_destroy(channel);
        ares_library_cleanup();
        LOG_DEBUG("Cleanup finished")
    }
}
