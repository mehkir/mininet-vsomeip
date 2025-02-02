#include "../include/statistics_writer.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>

std::mutex statistics_writer::mutex_;
statistics_writer* statistics_writer::instance_;
size_t statistics_writer::host_count_;
std::string statistics_writer::absolute_results_directory_path_;
std::string statistics_writer::result_filename_;

statistics_writer* statistics_writer::get_instance(size_t _host_count, std::string _absolute_results_directory_path, std::string _result_filename) {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    if(instance_ == nullptr) {
        instance_ = new statistics_writer();
        host_count_ = _host_count;
        absolute_results_directory_path_ = _absolute_results_directory_path;
        result_filename_ = _result_filename;
    }
    return instance_;
}

statistics_writer::statistics_writer() {
    time_metric_names_[time_metric::PUBLISHER_APP_INITIALIZATION_] = PUBLISHER_APP_INITIALIZATION;
    time_metric_names_[time_metric::SUBSCRIBER_APP_INITIALIZATION_START_] = SUBSCRIBER_APP_INITIALIZATION_START;
    time_metric_names_[time_metric::SUBSCRIBER_APP_INITIALIZATION_END_] = SUBSCRIBER_APP_INITIALIZATION_END;
    time_metric_names_[time_metric::GENERATE_OFFER_NONCE_START_] = GENERATE_OFFER_NONCE_START;
    time_metric_names_[time_metric::GENERATE_OFFER_NONCE_END_] = GENERATE_OFFER_NONCE_END;
    time_metric_names_[time_metric::FIND_SEND_] = FIND_SEND;
    time_metric_names_[time_metric::FIND_RECEIVE_] = FIND_RECEIVE;
    time_metric_names_[time_metric::OFFER_SEND_] = OFFER_SEND;
    time_metric_names_[time_metric::OFFER_RECEIVE_] = OFFER_RECEIVE;
    time_metric_names_[time_metric::SVCB_SERVICE_REQUEST_SEND_] = SVCB_SERVICE_REQUEST_SEND;
    time_metric_names_[time_metric::SVCB_SERVICE_REQUEST_RECEIVE_] = SVCB_SERVICE_REQUEST_RECEIVE;
    time_metric_names_[time_metric::SVCB_SERVICE_RESPONSE_SEND_] = SVCB_SERVICE_RESPONSE_SEND;
    time_metric_names_[time_metric::SVCB_SERVICE_RESPONSE_RECEIVE_] = SVCB_SERVICE_RESPONSE_RECEIVE;
    time_metric_names_[time_metric::VALIDATE_OFFER_START_] = VALIDATE_OFFER_START;
    time_metric_names_[time_metric::VALIDATE_OFFER_END_] = VALIDATE_OFFER_END;
    time_metric_names_[time_metric::CLIENT_SIGN_START_] = CLIENT_SIGN_START;
    time_metric_names_[time_metric::CLIENT_SIGN_END_] = CLIENT_SIGN_END;
    time_metric_names_[time_metric::SUBSCRIBE_SEND_] = SUBSCRIBE_SEND;
    time_metric_names_[time_metric::SUBSCRIBE_RECEIVE_] = SUBSCRIBE_RECEIVE;
    time_metric_names_[time_metric::SVCB_CLIENT_REQUEST_SEND_] = SVCB_CLIENT_REQUEST_SEND;
    time_metric_names_[time_metric::SVCB_CLIENT_REQUEST_RECEIVE_] = SVCB_CLIENT_REQUEST_RECEIVE;
    time_metric_names_[time_metric::SVCB_CLIENT_RESPONSE_SEND_] = SVCB_CLIENT_RESPONSE_SEND;
    time_metric_names_[time_metric::SVCB_CLIENT_RESPONSE_RECEIVE_] = SVCB_CLIENT_RESPONSE_RECEIVE;
    time_metric_names_[time_metric::TLSA_CLIENT_REQUEST_SEND_] = TLSA_CLIENT_REQUEST_SEND;
    time_metric_names_[time_metric::TLSA_CLIENT_REQUEST_RECEIVE_] = TLSA_CLIENT_REQUEST_RECEIVE;
    time_metric_names_[time_metric::TLSA_CLIENT_RESPONSE_SEND_] = TLSA_CLIENT_RESPONSE_SEND;
    time_metric_names_[time_metric::TLSA_CLIENT_RESPONSE_RECEIVE_] = TLSA_CLIENT_RESPONSE_RECEIVE;
    time_metric_names_[time_metric::VERIFY_CLIENT_SIGNATURE_START_] = VERIFY_CLIENT_SIGNATURE_START;
    time_metric_names_[time_metric::VERIFY_CLIENT_SIGNATURE_END_] = VERIFY_CLIENT_SIGNATURE_END;
    time_metric_names_[time_metric::SERVICE_SIGN_START_] = SERVICE_SIGN_START;
    time_metric_names_[time_metric::SERVICE_SIGN_END_] = SERVICE_SIGN_END;
    time_metric_names_[time_metric::SUBSCRIBE_ACK_SEND_] = SUBSCRIBE_ACK_SEND;
    time_metric_names_[time_metric::SUBSCRIBE_ACK_RECEIVE_] = SUBSCRIBE_ACK_RECEIVE;
    time_metric_names_[time_metric::TLSA_SERVICE_REQUEST_SEND_] = TLSA_SERVICE_REQUEST_SEND;
    time_metric_names_[time_metric::TLSA_SERVICE_REQUEST_RECEIVE_] = TLSA_SERVICE_REQUEST_RECEIVE;
    time_metric_names_[time_metric::TLSA_SERVICE_RESPONSE_SEND_] = TLSA_SERVICE_RESPONSE_SEND;
    time_metric_names_[time_metric::TLSA_SERVICE_RESPONSE_RECEIVE_] = TLSA_SERVICE_RESPONSE_RECEIVE;
    time_metric_names_[time_metric::VERIFY_SERVICE_SIGNATURE_START_] = VERIFY_SERVICE_SIGNATURE_START;
    time_metric_names_[time_metric::VERIFY_SERVICE_SIGNATURE_END_] = VERIFY_SERVICE_SIGNATURE_END;

    boost::interprocess::managed_shared_memory segment(boost::interprocess::create_only, SEGMENT_NAME, SEGMENT_SIZE_BYTES);
    void_allocator void_allocator_instance(segment.get_segment_manager());
    composite_time_statistics_ = segment.construct<shared_statistics_map>(TIME_STATISTICS_MAP_NAME)(std::less<host_key_t>(), void_allocator_instance);
    boost::interprocess::named_condition condition(boost::interprocess::create_only, STATISTICS_CONDITION);
    boost::interprocess::named_mutex mutex(boost::interprocess::create_only, STATISTICS_MUTEX);
}

statistics_writer::~statistics_writer() {
    boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only, SEGMENT_NAME);
    segment.destroy<shared_statistics_map>(TIME_STATISTICS_MAP_NAME);
}

void statistics_writer::write_statistics() {
    boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only, SEGMENT_NAME);
    composite_time_statistics_ = segment.find<shared_statistics_map>(TIME_STATISTICS_MAP_NAME).first;
    boost::interprocess::named_condition condition(boost::interprocess::open_only, STATISTICS_CONDITION);
    boost::interprocess::named_mutex mutex(boost::interprocess::open_only, STATISTICS_MUTEX);
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);
    while(!entries_are_complete()) {
        condition.notify_one();
        condition.wait(lock);
    }
    std::ofstream statistics_file;
    int filecount = 0;
    std::stringstream absolute_result_file_path;
    absolute_result_file_path << absolute_results_directory_path_ << result_filename_ << "-#" << filecount << ".csv";
    struct stat buffer;
    //Choose unused/non-existing absolute_result_file_path
    for(filecount = 1; (stat(absolute_result_file_path.str().c_str(), &buffer) == 0); filecount++) {
        absolute_result_file_path.str("");
        absolute_result_file_path << absolute_results_directory_path_ << result_filename_ << "-#" << filecount << ".csv";
    }
    statistics_file.open(absolute_result_file_path.str());
    //Write header
    statistics_file << "HOST,";
    for(metric_key_t metric_key = 0; metric_key < time_metric::TIME_METRIC_COUNT_; metric_key++) {
        statistics_file << time_metric_names_[metric_key];
        if(metric_key != time_metric::TIME_METRIC_COUNT_-1) {
            statistics_file << ",";
        } else {
            statistics_file << "\n";
        }
    }
    //Write values (keep metric order like above so that header and values comply)
    for(auto host_entry = composite_time_statistics_->begin(); host_entry != composite_time_statistics_->end(); host_entry++) {
        statistics_file << host_entry->first << ",";
        auto metrics_map = host_entry->second.metrics_map_;
        for(metric_key_t metric_key = 0; metric_key < time_metric::TIME_METRIC_COUNT_; metric_key++) {
            if(metrics_map.count(metric_key)) {
                statistics_file << metrics_map[metric_key];
            } else {
                statistics_file << 0;
            }
            if(metric_key != time_metric::TIME_METRIC_COUNT_-1) {
                statistics_file << ",";
            } else {
                statistics_file << "\n";
            }
        }
    }
    statistics_file.close();
}

bool statistics_writer::entries_are_complete() {
    bool entries_are_complete = false;
    size_t host_entry_count = composite_time_statistics_->size();
    // std::cout << __func__ << " " << host_entry_count << std::endl;
    // boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only, SEGMENT_NAME);
    // std::cout << __func__ << " free memory=" << segment.get_free_memory() << std::endl;
    for(auto host_entry = composite_time_statistics_->begin(); (host_entry_count == host_count_) && (host_entry != composite_time_statistics_->end()); host_entry++) {
        auto metrics_map = host_entry->second.metrics_map_;
#ifdef WITH_SERVICE_AUTHENTICATION
        entries_are_complete = metrics_map.count(time_metric::SUBSCRIBE_ACK_SEND_) && metrics_map.count(time_metric::VERIFY_SERVICE_SIGNATURE_END_);
#else
        entries_are_complete = metrics_map.count(time_metric::SUBSCRIBE_ACK_SEND_) && metrics_map.count(time_metric::SUBSCRIBE_ACK_RECEIVE_);
#endif
        if(!entries_are_complete)
            break;
    }
    return entries_are_complete;
}

void statistics_writer::print_statistics() {
    std::cout << __func__ << std::endl;
    std::stringstream sstream;
    sstream << "HOST,";
    for(metric_key_t metric_key = 0; metric_key < time_metric::TIME_METRIC_COUNT_; metric_key++) {
        sstream << time_metric_names_[metric_key];
        if(metric_key != time_metric::TIME_METRIC_COUNT_-1) {
            sstream << ",";
        } else {
            sstream << "\n";
        }
    }

    for(auto host_entry = composite_time_statistics_->begin(); host_entry != composite_time_statistics_->end(); host_entry++) {
        sstream << host_entry->first << ",";
        auto metrics_map = host_entry->second.metrics_map_;
        for(metric_key_t metric_key = 0; metric_key < time_metric::TIME_METRIC_COUNT_; metric_key++) {
            if(metrics_map.count(metric_key)) {
                sstream << metrics_map[metric_key];
            } else {
                sstream << 0;
            }
            if(metric_key != time_metric::TIME_METRIC_COUNT_-1) {
                sstream << ",";
            } else {
                sstream << "\n";
            }
        }
    }

    std::cout << sstream.str() << std::endl;
}