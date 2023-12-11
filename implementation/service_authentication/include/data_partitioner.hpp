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
    template <typename T> void partition_data(const std::string& _key_name, std::shared_ptr<vsomeip_v3::sd::configuration_option_impl> _configuration_option, 
                                       T _data);
    template <typename T> T reassemble_data(const std::string& _key_name, std::shared_ptr<vsomeip_v3::sd::configuration_option_impl> _configuration_option);
    std::string convert_to_comma_separated_int_string(std::vector<unsigned char> _data);
    std::vector<unsigned char> convert_comma_separated_string_to_unsigned_char_vector(std::string _comma_separated_string);
};

template <typename T> void data_partitioner::partition_data(const std::string& _key_name, std::shared_ptr<configuration_option_impl> _configuration_option,
                   T _data) {
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

template <typename T> T data_partitioner::reassemble_data(const std::string& _key_name, std::shared_ptr<vsomeip_v3::sd::configuration_option_impl> _configuration_option) {
    T reassembled_data;
    std::string partial_data;
    for(int partial_data_idx = 0; !(partial_data = _configuration_option->get_value(_key_name+std::to_string(partial_data_idx))).empty(); partial_data_idx++) {
        reassembled_data.insert(reassembled_data.end(), partial_data.begin(), partial_data.end());
    }
    return reassembled_data;
}

}
}
#endif /* VSOMEIP_DATA_PARTITIONER_HPP */