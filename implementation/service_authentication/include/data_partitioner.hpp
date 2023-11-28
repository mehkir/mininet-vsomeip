#ifndef VSOMEIP_DATA_PARTITIONER_HPP
#define VSOMEIP_DATA_PARTITIONER_HPP

#include <memory>
#include <vector>
#include "../../service_discovery/include/configuration_option_impl.hpp"

namespace vsomeip_v3 {
namespace sd {

class data_partitioner
{
private:

public:
    data_partitioner();
    ~data_partitioner();
    void partition_data(const std::string& _key_name, std::shared_ptr<vsomeip_v3::sd::configuration_option_impl> _configuration_option, 
                                       std::vector<unsigned char> _data);
    std::vector<unsigned char> reassemble_data(const std::string& _key_name, std::shared_ptr<vsomeip_v3::sd::configuration_option_impl> _configuration_option);
};
}
}
#endif /* VSOMEIP_DATA_PARTITIONER_HPP */