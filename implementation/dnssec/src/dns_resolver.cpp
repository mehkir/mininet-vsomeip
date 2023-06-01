//
// Created by mehkir on 21.09.22.
//

#include "../include/dns_resolver.hpp"
#include "../include/logger.hpp"
#include <arpa/inet.h>
#include <string.h>

#define EDNSPKSZ 1280 // https://datatracker.ietf.org/doc/html/rfc6891

std::mutex dns_resolver::mutex_;
dns_resolver* dns_resolver::instance_;

dns_resolver* dns_resolver::get_instance() {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    if(instance_ == nullptr) {
        instance_ = new dns_resolver();
    }
    return instance_;
}

void dns_resolver::resolve(const char* _name, int _dnsclass, int _type, ares_callback _callback, void* _arg) {
    if (!initialized_)
        throw std::runtime_error("dns_resolver is not initialized, call initialize() first!");
    dns_request _dns_request;
    char* url = (char*)malloc(strlen(_name)+1);
    strcpy(url, _name);
    _dns_request.name_ = url;
    _dns_request.dnsclass_ = _dnsclass;
    _dns_request.type_ = _type;
    _dns_request.callback_ = _callback;
    _dns_request.arg_ = _arg;
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        dns_requests_.push_back(_dns_request);
    }
    condition_variable_.notify_one();
}

int dns_resolver::change_dns_server(ares_channel& _channel, in_addr_t _address) {
    ares_addr_node servers;
    servers.next = nullptr;
    servers.family = AF_INET;
    servers.addr.addr4.s_addr = htonl(_address);
    return ares_set_servers(_channel, &servers);
}

int dns_resolver::initialize(in_addr_t _address) {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    if (!initialized_) {
        ares_library_init(ARES_LIB_INIT_ALL);
        if (ares_init(&channel_) != ARES_SUCCESS) {
            std::cout << "Channel initialization failed" << std::endl;
            return 1;
        }

        ares_options options;
        options.flags = 0;
        int optmask = 0;
        if (ares_save_options(channel_, &options, &optmask) != ARES_SUCCESS) {
            std::cout << "Retrieving options failed" << std::endl;
            return 1;
        }
        ares_destroy(channel_);
        options.flags |= ARES_FLAG_EDNS;
        optmask |= ARES_OPT_EDNSPSZ;
        options.ednspsz = EDNSPKSZ;
        if (ares_init_options(&channel_, &options, optmask) != ARES_SUCCESS) {
            std::cout << "Initializing with options failed" << std::endl;
            return 1;
        }
        ares_destroy_options(&options);
        if (change_dns_server(channel_, _address) != ARES_SUCCESS) {
            std::cout << "Setting servers failed" << std::endl;
            return 1;
        }
        state_ = STARTED;
        process_thread_ = std::thread(&dns_resolver::process, this);
        initialized_ = true;
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
    while (state_ != STOPPED) {
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
            std::unique_lock<std::mutex> unique_lock(mutex_);
            condition_variable_.wait(unique_lock, [this] { return !dns_requests_.empty() || state_ == STOPPED; });
        }
        /*
        if (waitBool)
            std::cout << "Process thread received signal. Processing ..." << std::endl;
        */
        if (state_ != STOPPED) {
            dns_request dns_request;
            {
                std::lock_guard<std::mutex> lockGuard(mutex_);
                dns_request = dns_requests_.front();
                dns_requests_.pop_front();
            }
            ares_search(channel_, dns_request.name_, dns_request.dnsclass_, dns_request.type_, dns_request.callback_,
                        dns_request.arg_);
            while (true) {
                FD_ZERO(&readers);
                FD_ZERO(&writers);
                nfds = ares_fds(channel_, &readers, &writers);
                if (nfds == 0)
                    break;
                tvp = ares_timeout(channel_, NULL, &tv);
                count = select(nfds, &readers, &writers, NULL, tvp);
                ares_process(channel_, &readers, &writers);
            }
            free(const_cast<char *>(dns_request.name_));

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
    if (initialized_) {
        LOG_DEBUG("Cleanup begins")
        state_ = STOPPED;
        condition_variable_.notify_one();
        process_thread_.join();
        ares_destroy(channel_);
        ares_library_cleanup();
        LOG_DEBUG("Cleanup finished")
    }
}
