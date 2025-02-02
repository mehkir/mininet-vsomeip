// Copyright (C) 2014-2023 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
#include <csignal>
#endif
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <fstream>

#include <vsomeip/vsomeip.hpp>

#include "sample-ids.hpp"

class my_publisher_app {
public:
    my_publisher_app(uint32_t _cycle) :
            app_(vsomeip::runtime::get()->create_application()),
            is_registered_(false),
            cycle_(_cycle),
            running_(true),
            is_offered_(false),
            notify_thread_(std::bind(&my_publisher_app::notify, this)) {
    }

    bool init() {
        if (!app_->init()) {
            std::cerr << "Couldn't initialize application" << std::endl;
            return false;
        }
        app_->register_state_handler(
                std::bind(&my_publisher_app::on_state, this,
                        std::placeholders::_1));

        std::set<vsomeip::eventgroup_t> its_groups;
        its_groups.insert(SAMPLE_EVENTGROUP_ID);
        app_->offer_event(
                SAMPLE_SERVICE_ID,
                SAMPLE_INSTANCE_ID,
                SAMPLE_EVENT_ID,
                its_groups,
                vsomeip::event_type_e::ET_FIELD, std::chrono::milliseconds::zero(),
                false, true, nullptr, vsomeip::reliability_type_e::RT_UNKNOWN);
        {
            std::lock_guard<std::mutex> its_lock(payload_mutex_);
            payload_ = vsomeip::runtime::get()->create_payload();
        }

        offer();
        return true;
    }

    void start() {
        app_->start();
    }

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
    /*
     * Handle signal to shutdown
     */
    void stop() {
        running_ = false;
        notify_condition_.notify_one();
        app_->clear_all_handler();
        stop_offer();
        if (std::this_thread::get_id() != notify_thread_.get_id()) {
            if (notify_thread_.joinable()) {
                notify_thread_.join();
            }
        } else {
            notify_thread_.detach();
        }
        app_->stop();
    }
#endif

    void offer() {
        std::lock_guard<std::mutex> its_lock(notify_mutex_);
        app_->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
        is_offered_ = true;
        notify_condition_.notify_one();
    }

    void stop_offer() {
        app_->stop_offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
        is_offered_ = false;
    }

    void on_state(vsomeip::state_type_e _state) {
        // std::cout << "Application " << app_->get_name() << " is "
        // << (_state == vsomeip::state_type_e::ST_REGISTERED ?
        //         "registered." : "deregistered.") << std::endl;

        if (_state == vsomeip::state_type_e::ST_REGISTERED) {
            if (!is_registered_) {
                is_registered_ = true;
            }
        } else {
            is_registered_ = false;
        }
    }

    void notify() {
        vsomeip::byte_t its_data[10];
        uint32_t its_size = 1;

        while (running_) {
            std::unique_lock<std::mutex> its_lock(notify_mutex_);
            while (!is_offered_ && running_)
                notify_condition_.wait(its_lock);
            while (is_offered_ && running_) {
                if (its_size == sizeof(its_data))
                    its_size = 1;

                // std::cout << "Generating payload = (" << its_size << ") ";
                for (uint32_t i = 0; i < its_size; ++i) {
                    its_data[i] = static_cast<uint8_t>(i);
                    // std::cout << i << " ";
                }
                // std::cout << std::endl;

                {
                    std::lock_guard<std::mutex> its_lock(payload_mutex_);
                    payload_->set_data(its_data, its_size);

                    app_->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload_);
                }

                its_size++;

                std::this_thread::sleep_for(std::chrono::milliseconds(cycle_));
            }
        }
    }

private:
    std::shared_ptr<vsomeip::application> app_;
    bool is_registered_;
    uint32_t cycle_;

    bool running_;

    std::mutex notify_mutex_;
    std::condition_variable notify_condition_;
    bool is_offered_;

    std::mutex payload_mutex_;
    std::shared_ptr<vsomeip::payload> payload_;

    std::thread notify_thread_;
};

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
    my_publisher_app *publisher_app_ptr(nullptr);
    void handle_signal(int _signal) {
        if (publisher_app_ptr != nullptr &&
                (_signal == SIGINT || _signal == SIGTERM))
            publisher_app_ptr->stop();
    }
#endif

int main(int argc, char **argv) {
    uint32_t cycle = 1000; // default 1s

    std::string cycle_arg("--cycle");

    for (int i = 1; i < argc; i++) {
        if (cycle_arg == argv[i] && i + 1 < argc) {
            i++;
            std::stringstream converter;
            converter << argv[i];
            converter >> cycle;
        }
    }

    my_publisher_app publisher_app(cycle);
#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
    publisher_app_ptr = &publisher_app;
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
#endif
    if (publisher_app.init()) {
        std::ofstream publisher_initialized_file;
        publisher_initialized_file.open("/home/mehmet/vscode-workspaces/mininet-vsomeip/publisher-initialized");
        publisher_initialized_file.close();
        publisher_app.start();
        return 0;
    } else {
        return 1;
    }
}