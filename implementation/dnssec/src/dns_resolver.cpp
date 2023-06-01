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

dns_resolver* dns_resolver::get_instance() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if(instance == nullptr) {
        instance = new dns_resolver();
    }
    return instance;
}

void dns_resolver::resolve(const char *name, int dnsclass, int type, ares_callback callback, void *arg) {
    if (!initialized)
        throw std::runtime_error("dns_resolver is not initialized, call initialize() first!");
    dns_request _dns_request;
    char* url = (char*)malloc(strlen(name)+1);
    strcpy(url, name);
    _dns_request.name = url;
    _dns_request.dnsclass = dnsclass;
    _dns_request.type = type;
    _dns_request.callback = callback;
    _dns_request.arg = arg;
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        dns_requests_.push_back(_dns_request);
    }
    conditionVariable.notify_one();
}

int dns_resolver::change_dns_server(ares_channel& channel, in_addr_t address) {
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
        if (change_dns_server(channel, address) != ARES_SUCCESS) {
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
        if (dns_requests_.empty()) {
            std::cout << "Process thread waits for data" << std::endl;
            waitBool = true;
        } else {
            waitBool = false;
        }
         */
        {
            std::unique_lock<std::mutex> uniqueLock(mutex_);
            conditionVariable.wait(uniqueLock, [this] { return !dns_requests_.empty() || state == STOPPED; });
        }
        /*
        if (waitBool)
            std::cout << "Process thread received signal. Processing ..." << std::endl;
        */
        if (state != STOPPED) {
            dns_request _dns_request;
            {
                std::lock_guard<std::mutex> lockGuard(mutex_);
                _dns_request = dns_requests_.front();
                dns_requests_.pop_front();
            }
            ares_search(channel, _dns_request.name, _dns_request.dnsclass, _dns_request.type, _dns_request.callback,
                        _dns_request.arg);
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
            free(const_cast<char *>(_dns_request.name));

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
