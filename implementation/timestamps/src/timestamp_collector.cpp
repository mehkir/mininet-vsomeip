#include "../include/timestamp_collector.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <vector>
#include <stdexcept>

namespace vsomeip_v3 {

    std::mutex timestamp_collector::mutex_;
    timestamp_collector* timestamp_collector::instance_;

    timestamp_collector* timestamp_collector::get_instance() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if(instance_ == nullptr) {
        instance_ = new timestamp_collector();
    }
    return instance_;
}

    timestamp_collector::timestamp_collector() {
    }

    timestamp_collector::~timestamp_collector() {
    }

    void timestamp_collector::record_timestamp(std::string timepoint) {
        if(timestamps_.count(timepoint)) {
            throw std::runtime_error("There is already a timestamp for the key: " + timepoint);
        }
        timestamps_[timepoint] = std::chrono::system_clock::now();
    }

    void timestamp_collector::write_timestamps(NODES node) {
        std::vector<std::string> publisher_timepoints = {SUBSCRIBE_ARRIVED, SIGNING_START,
                                              SIGNING_END, SUBSCRIBE_ACK_SEND};
        std::vector<std::string> subscriber_timepoints = {APPLICATION_INIT, SVCB_REQUEST, SVCB_RESPONSE, TLSA_REQUEST, TLSA_RESPONSE,
                         SUBSCRIBE_SEND, SUBSCRIBE_ACK_ARRIVED, CHECK_SIGNATURE_START, CHECK_SIGNATURE_END};
        std::vector<std::string> timepoints;
        switch (node)
        {
        case PUBLISHER:
            timepoints = publisher_timepoints;
            break;
        case SUBSCRIBER:
            timepoints = subscriber_timepoints;
            break;
        default:
            break;
        }
        std::ofstream timepoints_file;
        int filecount = 0;
        std::stringstream filename;
        filename << "timestamp_results/timepoints-#" << filecount << ".csv";
        struct stat buffer;
        for(filecount = 1; (stat(filename.str().c_str(), &buffer) == 0); filecount++) {
            filename.str("");
            filename << "timestamp_results/timepoints-#" << filecount << ".csv";
        }
        timepoints_file.open(filename.str());
        //Write header
        for(size_t timepoint_count = 0; timepoint_count < timepoints.size(); timepoint_count++) {
            timepoints_file << timepoints[timepoint_count];
            if(timepoint_count != timepoints.size()-1) {
                timepoints_file << ",";
            } else {
                timepoints_file << "\n";
            }
        }
        //Write values
        for(size_t timepoint_count = 0; timepoint_count < timepoints.size(); timepoint_count++) {
            timepoints_file << timestamps_[timepoints[timepoint_count]].time_since_epoch().count();
            if(timepoint_count != timepoints.size()-1) {
                timepoints_file << ",";
            } else {
                timepoints_file << "\n";
            }
        }
        timepoints_file.close();
    }
}