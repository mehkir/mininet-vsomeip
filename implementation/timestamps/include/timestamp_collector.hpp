#ifndef VSOMEIP_V3_TIMESTAMP_COLLECTOR_HPP_
#define VSOMEIP_V3_TIMESTAMP_COLLECTOR_HPP_

// Subscriber Timepoints for Timestamps
#define APPLICATION_INIT      "APPLICATION_INIT"
#define SVCB_REQUEST          "SVCB_REQUEST" 
#define SVCB_RESPONSE         "SVCB_RESPONSE"
#define TLSA_REQUEST          "TLSA_REQUEST"
#define TLSA_RESPONSE         "TLSA_RESPONSE"
#define SUBSCRIBE_SEND        "SUBSCRIBE_SEND"
#define SUBSCRIBE_ACK_ARRIVED "SUBSCRIBE_ACK_ARRIVED"
#define CHECK_SIGNATURE_START "CHECK_SIGNATURE_START"
#define CHECK_SIGNATURE_END   "CHECK_SIGNATURE_END"
// Publisher Timepoints for Timestamps
#define SUBSCRIBE_ARRIVED     "SUBSCRIBE_ARRIVED"
#define SIGNING_START         "SIGNING_START"
#define SIGNING_END           "SIGNING_END"
#define SUBSCRIBE_ACK_SEND    "SUBSCRIBE_ACK_SEND"


#include <map>
#include <mutex>
#include <chrono>

namespace vsomeip_v3 {
    
    enum NODES {
        PUBLISHER,
        SUBSCRIBER
    };

    class timestamp_collector
    {
    private:
        static std::mutex mutex_;
        static timestamp_collector* instance_;
        std::map<std::string,std::chrono::time_point<std::chrono::system_clock>> timestamps_;
        timestamp_collector();
        ~timestamp_collector();
    public:
        static timestamp_collector* get_instance();

        void record_timestamp(std::string timepoint);
        void write_timestamps(NODES node);
    };
    

}

#endif