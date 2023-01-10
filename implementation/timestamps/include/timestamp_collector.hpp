#ifndef VSOMEIP_V3_TIMESTAMP_COLLECTOR_HPP_
#define VSOMEIP_V3_TIMESTAMP_COLLECTOR_HPP_

#define SERVICE_REQUEST       "SERVICE_REQUEST"
#define SVCB_REQUEST          "SVCB_REQUEST" 
#define SVCB_RESPONSE         "SVCB_RESPONSE"
#define TLSA_REQUEST          "TLSA_REQUEST"
#define TLSA_RESPONSE         "TLSA_RESPONSE"
#define SUBSCRIBE             "SUBSCRIBE"
#define SUBSCRIBE_ACK         "SUBSCRIBE_ACK"
#define SIGNING_START         "SIGNING_START"
#define SIGNING_END           "SIGNING_END"
#define CHECK_SIGNATURE_START "CHECK_SIGNATURE_START"
#define CHECK_SIGNATURE_END   "CHECK_SIGNATURE_END"

#include <map>
#include <mutex>
#include <chrono>

namespace vsomeip_v3 {

    class timestamp_collector
    {
    private:
        static std::mutex mutex_;
        static timestamp_collector* instance;
        std::map<std::string,std::chrono::time_point<std::chrono::system_clock>> timestamps_;
        timestamp_collector();
        ~timestamp_collector();
    public:
        static timestamp_collector* getInstance();

        void record_timestamp(std::string timepoint);
        void write_timestamps();
    };
    

}

#endif