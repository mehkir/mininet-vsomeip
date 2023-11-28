#include "../include/data_partitioner.hpp"

#define CERTKEY "cert"

namespace vsomeip_v3 {
namespace sd {

data_partitioner::data_partitioner() {
}

data_partitioner::~data_partitioner() {
}

void data_partitioner::partition_data(const std::string& _key_name, std::shared_ptr<configuration_option_impl> _configuration_option,
                   std::vector<unsigned char> _data) {
    size_t remaining_bytes = _data.size();
    size_t data_partition_pos = 0;
    int maximum_key_value_pair_size = 0xFF;
    std::string key_name;
    for(size_t data_key_idx = 0; remaining_bytes > 0; data_key_idx++) {
        key_name = _key_name+std::to_string(data_key_idx);
        size_t allowed_size = maximum_key_value_pair_size-key_name.size()-1; // 1 for equal sign
        if (remaining_bytes < allowed_size) {allowed_size = remaining_bytes;}
        _configuration_option->add_item(key_name, std::string(_data.begin()+data_partition_pos,_data.begin()+(allowed_size+data_partition_pos)));
        remaining_bytes = remaining_bytes - allowed_size;
        data_partition_pos = data_partition_pos + allowed_size;
    }
}

std::vector<unsigned char> data_partitioner::reassemble_data(const std::string& _key_name, std::shared_ptr<vsomeip_v3::sd::configuration_option_impl> _configuration_option) {
    std::vector<unsigned char> reassembled_data;
    std::string partial_data;
    for(int partial_data_idx = 0; !(partial_data = _configuration_option->get_value(_key_name+std::to_string(partial_data_idx))).empty(); partial_data_idx++) {
        reassembled_data.insert(reassembled_data.end(), partial_data.begin(), partial_data.end());
    }
    return reassembled_data;
}

}
}