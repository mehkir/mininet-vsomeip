#include "../include/payload_key_value_encoder.hpp"

payload_key_value_encoder::payload_key_value_encoder() {

}

payload_key_value_encoder::~payload_key_value_encoder() {

}

void payload_key_value_encoder::add_item(const std::string& _key_name, const std::string& _value) const {
    size_t remaining_bytes = _value.size();
    size_t data_partition_pos = 0;
    int maximum_key_value_pair_size = 0xFF; // DNS-Based SD (rfc6763) maximum of 255 bit of text data after 1 byte length field
    std::string key_name;
    for(size_t data_key_idx = 0; remaining_bytes > 0; data_key_idx++) {
        key_name = _key_name+std::to_string(data_key_idx);
        size_t allowed_size = maximum_key_value_pair_size-key_name.size()-1; // 1 for equal sign
        if (remaining_bytes < allowed_size) {allowed_size = remaining_bytes;}
        std::string partioned_data(_value.begin()+data_partition_pos,_value.begin()+(allowed_size+data_partition_pos));
        plain_key_value_map_[key_name] = partioned_data;
        remaining_bytes = remaining_bytes - allowed_size;
        data_partition_pos = data_partition_pos + allowed_size;
    }
}

std::string payload_key_value_encoder::encode() const {
    std::string encoded_payload_str;
    for (auto key_value_pair : plain_key_value_map_) {
        std::string key_name = key_value_pair.first;
        std::string value = key_value_pair.second;
        uint8_t item_length = (uint8_t) (key_name.size()+1+value.size()); // 1 for equal sign
        encoded_payload_str.append(reinterpret_cast<const char*>(&item_length), sizeof(item_length));
        encoded_payload_str.append(key_name);
        encoded_payload_str.append("=");
        encoded_payload_str.append(value);
    }
    plain_key_value_map_.clear();
    return encoded_payload_str;
}