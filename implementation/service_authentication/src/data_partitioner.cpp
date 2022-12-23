#include "../include/data_partitioner.hpp"

#define CERTKEY "cert"

namespace vsomeip_v3::sd {

data_partitioner::data_partitioner() {
}

data_partitioner::~data_partitioner() {
}

void data_partitioner::partition_data(const std::string& key_name, std::shared_ptr<configuration_option_impl> configuration_option,
                   std::vector<unsigned char> data) {
    int remainingBytes = data.size();
    int dataPartitionPos = 0;
    int maximumKeyValuePairSize = 0xFF;
    std::string keyName;
    for(int dataKeyIdx = 0; remainingBytes > 0; dataKeyIdx++) {
        keyName = key_name+std::to_string(dataKeyIdx);
        int allowedSize = maximumKeyValuePairSize-keyName.size()-1; // 1 for equal sign
        if (remainingBytes < allowedSize) {allowedSize = remainingBytes;}
        configuration_option->add_item(keyName, std::string((char*)data.data(), dataPartitionPos, allowedSize));
        remainingBytes = remainingBytes - allowedSize;
        dataPartitionPos = dataPartitionPos + allowedSize;
    }
}

std::vector<unsigned char> data_partitioner::reassemble_data(const std::string& key_name, std::shared_ptr<vsomeip_v3::sd::configuration_option_impl> configuration_option) {
    std::vector<unsigned char> reassembled_data;
    std::string partial_data;
    for(int partial_data_idx = 0; !(partial_data = configuration_option->get_value(key_name+std::to_string(partial_data_idx))).empty(); partial_data_idx++) {
        reassembled_data.insert(reassembled_data.end(), partial_data.begin(), partial_data.end());
    }
    return reassembled_data;
}

}