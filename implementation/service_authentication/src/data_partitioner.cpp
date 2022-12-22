#include "../include/data_partitioner.hpp"

#define CERTKEY "cert"

namespace vsomeip_v3::sd {

data_partitioner::data_partitioner() {
}

data_partitioner::~data_partitioner() {
}

void data_partitioner::partitionCertificateData(std::shared_ptr<configuration_option_impl> configuration_option,
                   std::vector<unsigned char> data) {
    int remainingBytes = data.size();
    int dataPartitionPos = 0;
    int maximumKeyValuePairSize = 0xFF;
    std::string keyName;
    for(int dataKeyIdx = 0; remainingBytes > 0; dataKeyIdx++) {
        keyName = CERTKEY+std::to_string(dataKeyIdx);
        int allowedSize = maximumKeyValuePairSize-keyName.size()-1; // 1 for equal sign
        if (remainingBytes < allowedSize)
            allowedSize = remainingBytes;
        configuration_option->add_item(keyName, std::string((char*)data.data(), dataPartitionPos, allowedSize));
        remainingBytes = remainingBytes - allowedSize;
        dataPartitionPos = dataPartitionPos + allowedSize;
    }
}

std::vector<unsigned char> data_partitioner::reassembleCertificateData(std::shared_ptr<vsomeip_v3::sd::configuration_option_impl> configuration_option) {
    std::vector<unsigned char> reassembledCertificateData;
    std::string certificatePartition;
    for(int certPartitionIdx = 0; (certificatePartition = configuration_option->get_value(CERTKEY+std::to_string(certPartitionIdx))).empty(); certPartitionIdx++) {
        reassembledCertificateData.insert(reassembledCertificateData.end(), certificatePartition.begin(), certificatePartition.end());
    }
    return reassembledCertificateData;
}

}