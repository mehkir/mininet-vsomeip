#include "../include/statistics_recorder.hpp"

#include <iostream>
#include <stdexcept>

std::mutex statistics_recorder::mutex_;
statistics_recorder* statistics_recorder::instance_;

statistics_recorder* statistics_recorder::get_instance() {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    if(instance_ == nullptr) {
        instance_ = new statistics_recorder();
    }
    return instance_;
}

statistics_recorder::statistics_recorder() {
}

statistics_recorder::~statistics_recorder() {
}

void statistics_recorder::record_timestamp(uint32_t _host_ip, time_metric _time_metric) {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    if(!time_statistics_.count(_host_ip) || !time_statistics_[_host_ip].count(_time_metric)) {
        time_statistics_[_host_ip][_time_metric] = std::chrono::system_clock::now().time_since_epoch().count();
    }
}

void statistics_recorder::contribute_statistics() {
    bool waited_for_shm = false;
    for (bool shared_objects_initialized = false; !shared_objects_initialized;) {
        try {
            boost::interprocess::named_condition condition(boost::interprocess::open_only, STATISTICS_CONDITION);
            boost::interprocess::named_mutex mutex(boost::interprocess::open_only, STATISTICS_MUTEX);
            boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only, SEGMENT_NAME);
            void_allocator void_allocator_instance(segment.get_segment_manager());
            boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);

            while (!(composite_time_statistics_ = segment.find<shared_statistics_map>(TIME_STATISTICS_MAP_NAME).first)) {
                waited_for_shm = true;
                condition.wait(lock);
                std::cout << "[<statistics_recorder>] (" << __func__ << ") shared maps not intialized yet" << std::endl;
            }
            if(waited_for_shm) {
                std::cout << "[<statistics_recorder>] (" << __func__ << ") resume composing" << std::endl;
            }

            for(auto host_entry : time_statistics_) {
                metrics_map_data* mapped_metrics_map;
                if(composite_time_statistics_->count(host_entry.first)) {
                    mapped_metrics_map = &composite_time_statistics_->at(host_entry.first);
                } else {
                    metrics_map_data metrics_map_data_var = metrics_map_data(void_allocator_instance);
                    mapped_metrics_map = &metrics_map_data_var;
                }
                for(auto metrics_entry : host_entry.second) {
                    mapped_metrics_map->metrics_map_.insert({metrics_entry.first, metrics_entry.second});
                }
                composite_time_statistics_->insert({host_entry.first, *mapped_metrics_map});
            }
            condition.notify_one();
            shared_objects_initialized = true;
        } catch (boost::interprocess::interprocess_exception& interprocess_exception) {
            std::cerr << interprocess_exception.what() << std::endl;
            std::cout << "[<statistics_recorder>] (" << __func__ << ") shared objects not created yet" << std::endl;
            sleep(1);
        }
    }
}