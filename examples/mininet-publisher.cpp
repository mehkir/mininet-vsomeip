// Copyright (C) 2014-2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
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

#include <vsomeip/vsomeip.hpp>

#include "multiple-services-ids.hpp"

class multiple_notifications {
public:
    multiple_notifications(uint32_t _cycle) :
            app_(vsomeip::runtime::get()->create_application()),
            is_registered_(false),
            cycle_(_cycle),
            blocked_(false),
            running_(true),
            is_offered_(false),
            offer_thread_(std::bind(&multiple_notifications::run, this)),
            notify_thread_(std::bind(&multiple_notifications::notify, this)) {
    }

    bool init() {
        std::lock_guard<std::mutex> its_lock(mutex_);

        if (!app_->init()) {
            std::cerr << "Couldn't initialize application" << std::endl;
            return false;
        }
        app_->register_state_handler(
                std::bind(&multiple_notifications::on_state, this,
                        std::placeholders::_1));

        // Register handlers for speed service
        app_->register_message_handler(
                SPEED_SERVICE_ID,
                SPEED_INSTANCE_ID,
                SPEED_GET_METHOD_ID,
                std::bind(&multiple_notifications::on_get, this,
                          std::placeholders::_1));

        app_->register_message_handler(
                SPEED_SERVICE_ID,
                SPEED_INSTANCE_ID,
                SPEED_SET_METHOD_ID,
                std::bind(&multiple_notifications::on_set, this,
                          std::placeholders::_1));

        // Offer speed service
        std::set<vsomeip::eventgroup_t> its_speed_groups;
        its_speed_groups.insert(SPEED_EVENTGROUP_ID);
        app_->offer_event(
                SPEED_SERVICE_ID,
                SPEED_INSTANCE_ID,
                SPEED_EVENT_ID,
                its_speed_groups,
                vsomeip::event_type_e::ET_FIELD, std::chrono::milliseconds::zero(),
                false, true, nullptr, vsomeip::reliability_type_e::RT_UNKNOWN);
        {
            std::lock_guard<std::mutex> its_lock(payload_mutex_);
            payload_ = vsomeip::runtime::get()->create_payload();
        }

        blocked_ = true;
        condition_.notify_one();
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
        blocked_ = true;
        condition_.notify_one();
        notify_condition_.notify_one();
        app_->clear_all_handler();
        stop_offer();
        offer_thread_.join();
        notify_thread_.join();
        app_->stop();
    }
#endif

    void offer() {
        std::lock_guard<std::mutex> its_lock(notify_mutex_);
        app_->offer_service(SPEED_SERVICE_ID, SPEED_INSTANCE_ID);
        is_offered_ = true;
        notify_condition_.notify_one();
    }

    void stop_offer() {
        app_->stop_offer_service(SPEED_SERVICE_ID, SPEED_INSTANCE_ID);
        is_offered_ = false;
    }

    /*
     * Callback when app de-/registers
     */
    void on_state(vsomeip::state_type_e _state) {
        std::cout << "Application " << app_->get_name() << " is "
        << (_state == vsomeip::state_type_e::ST_REGISTERED ?
                "registered." : "deregistered.") << std::endl;

        if (_state == vsomeip::state_type_e::ST_REGISTERED) {
            if (!is_registered_) {
                is_registered_ = true;
            }
        } else {
            is_registered_ = false;
        }
    }

    void on_get(const std::shared_ptr<vsomeip::message> &_message) {
        /*
        std::stringstream its_message;
        its_message << "on_get for Event ["
                << std::setw(4)    << std::setfill('0') << std::hex
                << _message->get_service() << "."
                << std::setw(4) << std::setfill('0') << std::hex
                << _message->get_instance() << "."
                << std::setw(4) << std::setfill('0') << std::hex
                << _message->get_method() << "] to Client/Session ["
                << std::setw(4) << std::setfill('0') << std::hex
                << _message->get_client() << "/"
                << std::setw(4) << std::setfill('0') << std::hex
                << _message->get_session()
                << "]";
        std::cout << its_message.str() << std::endl;
        */

        std::shared_ptr<vsomeip::message> its_response
            = vsomeip::runtime::get()->create_response(_message);
        {
            std::lock_guard<std::mutex> its_lock(payload_mutex_);
            its_response->set_payload(payload_);
            /*its_response->set_instance(_message->get_instance());*/
        }
        app_->send(its_response);
    }

    void on_set(const std::shared_ptr<vsomeip::message> &_message) {
        /*
        std::stringstream its_message;
        its_message << "on_set for Event ["
                << std::setw(4)    << std::setfill('0') << std::hex
                << _message->get_service() << "."
                << std::setw(4) << std::setfill('0') << std::hex
                << _message->get_instance() << "."
                << std::setw(4) << std::setfill('0') << std::hex
                << _message->get_method() << "] to Client/Session ["
                << std::setw(4) << std::setfill('0') << std::hex
                << _message->get_client() << "/"
                << std::setw(4) << std::setfill('0') << std::hex
                << _message->get_session()
                << "]";
        std::cout << its_message.str() << std::endl;
        */

        std::shared_ptr<vsomeip::message> its_response
            = vsomeip::runtime::get()->create_response(_message);
        {
            std::lock_guard<std::mutex> its_lock(payload_mutex_);
            payload_ = _message->get_payload();
            its_response->set_payload(payload_);
        }

        app_->send(its_response);
        app_->notify(SPEED_SERVICE_ID, SPEED_INSTANCE_ID,
                     SPEED_EVENT_ID, payload_);
    }

    void run() {
        std::unique_lock<std::mutex> its_lock(mutex_);
        while (!blocked_)
            condition_.wait(its_lock);
        while(running_) {
            offer();
            std::this_thread::sleep_for(std::chrono::seconds(10));
            stop_offer();
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }

        /*
        bool is_offer(true);
        while (running_) {
            if (is_offer)
                offer();
            else
                stop_offer();

            for (int i = 0; i < 10 && running_; i++)
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            is_offer = !is_offer;
        }
        */
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

                for (uint32_t i = 0; i < its_size; ++i)
                    its_data[i] = static_cast<uint8_t>(i);

                {
                    std::lock_guard<std::mutex> its_lock(payload_mutex_);
                    payload_->set_data(its_data, its_size);

                    std::cout << "Setting event (Length=" << std::dec << its_size << ")." << std::endl;
                    app_->notify(SPEED_SERVICE_ID, SPEED_INSTANCE_ID, SPEED_EVENT_ID, payload_);
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

    std::mutex mutex_;
    std::condition_variable condition_;
    bool blocked_;
    bool running_;

    std::mutex notify_mutex_;
    std::condition_variable notify_condition_;
    bool is_offered_;

    std::mutex payload_mutex_;
    std::shared_ptr<vsomeip::payload> payload_;

    // blocked_ / is_offered_ must be initialized before starting the threads!
    std::thread offer_thread_;
    std::thread notify_thread_;
};

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
    multiple_notifications *its_sample_ptr(nullptr);
    void handle_signal(int _signal) {
        if (its_sample_ptr != nullptr &&
                (_signal == SIGINT || _signal == SIGTERM))
            its_sample_ptr->stop();
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

    multiple_notifications its_sample(cycle);
#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
    its_sample_ptr = &its_sample;
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
#endif
    if (its_sample.init()) {
        its_sample.start();
        return 0;
    } else {
        return 1;
    }
}
