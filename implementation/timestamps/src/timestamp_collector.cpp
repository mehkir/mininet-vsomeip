#include "../include/timestamp_collector.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace vsomeip_v3 {

    timestamp_collector::timestamp_collector() {
    }

    timestamp_collector::~timestamp_collector() {
    }

    void timestamp_collector::record_timestamp(std::string timepoint) {
        timestamps_[timepoint] = std::chrono::system_clock::now();
    }

    void timestamp_collector::write_timestamps() {
        std::string timepoints[] = {SERVICE_REQUEST, SVCB_REQUEST, SVCB_RESPONSE, TLSA_REQUEST, TLSA_RESPONSE,
                         SUBSCRIBE, SUBSCRIBE_ACK, SIGNING_START, SIGNING_END, CHECK_SIGNATURE_START, CHECK_SIGNATURE_END};
        std::ofstream timepoints_file;
        int filecount = 0;
        std::stringstream filename;
        filename << "timepoints #" << filecount << ".csv";
        struct stat buffer;
        for(filecount = 1; (stat(filename.str().c_str(), &buffer) == 0); filecount++) {
            filename.clear();
            filename << "timepoints #" << filecount << ".csv";
        }
        timepoints_file.open(filename.str());
        //Write header
        for(int timepoint_count = 0; timepoint_count < timepoints->size(); timepoint_count++) {
            timepoints_file << timepoints[timepoint_count];
            if(timepoint_count != timepoints->size()-1) {
                timepoints_file << ",";
            } else {
                timepoints_file << "\n";
            }
        }
        //Write values
        for(int timepoint_count = 0; timepoint_count < timepoints->size(); timepoint_count++) {
            timepoints_file << timestamps_[timepoints[timepoint_count]].time_since_epoch().count();
            if(timepoint_count != timepoints->size()-1) {
                timepoints_file << ",";
            } else {
                timepoints_file << "\n";
            }
        }
        timepoints_file.close();
    }
}