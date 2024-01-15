#ifndef VSOMEIP_V3_STATISTICS_WRITER
#define VSOMEIP_V3_STATISTICS_WRITER

#include "shared_memory_parameters.hpp"

struct shm_remove
{
    shm_remove() { boost::interprocess::shared_memory_object::remove(SEGMENT_NAME); }
    ~shm_remove(){ boost::interprocess::shared_memory_object::remove(SEGMENT_NAME); }
};

struct mutex_remove
{
    mutex_remove() { boost::interprocess::named_mutex::remove(STATISTICS_MUTEX); }
    ~mutex_remove(){ boost::interprocess::named_mutex::remove(STATISTICS_MUTEX); }
};

struct condition_remove
{
    condition_remove() { boost::interprocess::named_condition::remove(STATISTICS_CONDITION); }
    ~condition_remove(){ boost::interprocess::named_condition::remove(STATISTICS_CONDITION); }
};

class statistics_writer {
public:
    static statistics_writer* get_instance(int _host_count, std::string _absolute_project_path, std::string _result_filename);
    void write_statistics();
    ~statistics_writer();
private:
    static std::mutex mutex_;
    static statistics_writer* instance_;
    static int host_count_;
    static std::string absolute_results_directory_path_;
    static std::string result_filename_;
    shm_remove shm_remover_;
    mutex_remove mutex_remover_;
    condition_remove condition_remover_;
    std::unordered_map<metric_key_t, std::string> time_metric_names_;
    shared_statistics_map* composite_time_statistics_;
    statistics_writer();
};

#endif /* VSOMEIP_V3_STATISTICS_WRITER */