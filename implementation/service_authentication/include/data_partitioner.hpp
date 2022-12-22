#ifndef VSOMEIP_DATA_PARTITIONER_HPP
#define VSOMEIP_DATA_PARTITIONER_HPP

#include <memory>
#include <vector>
#include "../../service_discovery/include/configuration_option_impl.hpp"

namespace vsomeip_v3::sd {

class data_partitioner
{
private:
    /* data */
public:
    data_partitioner();
    ~data_partitioner();
    void partitionData(std::shared_ptr<vsomeip_v3::sd::configuration_option_impl> configuration_option, 
                                       std::vector<unsigned char> data);
};
}
#endif /* VSOMEIP_DATA_PARTITIONER_HPP */