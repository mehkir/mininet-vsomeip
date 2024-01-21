#ifndef VSOMEIP_V3_STATISTICS_RECORDER
#define VSOMEIP_V3_STATISTICS_RECORDER

#include "shared_memory_parameters.hpp"

class statistics_recorder
{
public:
    static statistics_recorder* get_instance();
    void record_timestamp(uint32_t _host_ip, time_metric _time_metric);
    void record_custom_timestamp(uint32_t _host_ip, time_metric _time_metric, uint64_t _timestamp);
    void contribute_statistics();
    ~statistics_recorder();
private:
    static std::mutex mutex_;
    static statistics_recorder* instance_;
    std::unordered_map<host_key_t, std::unordered_map<metric_key_t, metric_value_t>> time_statistics_;
    shared_statistics_map* composite_time_statistics_;
    statistics_recorder();
};

#endif /* VSOMEIP_V3_STATISTICS_RECORDER */