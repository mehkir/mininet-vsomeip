#include "../include/payload_key_value_decoder.hpp"

payload_key_value_decoder::payload_key_value_decoder() {

}

payload_key_value_decoder::~payload_key_value_decoder() {

}

void payload_key_value_decoder::decode(std::string& _encoded_payload_data) {
    decoded_key_value_map_.clear();
    for (auto it = _encoded_payload_data.begin(); it != _encoded_payload_data.end(); it++ ) {
        size_t item_length = (std::uint8_t) *it;
        it++;
        std::string item(it,it+item_length);
        size_t equal_char_idx = item.find('=');
        std::string key_name = item.substr(0, equal_char_idx);
        std::string value = item.substr(equal_char_idx + 1);
        decoded_key_value_map_[key_name] = value;
        it+=item_length-1; // -1 since loop increments by one already
    }
}

std::string payload_key_value_decoder::get_item(const std::string& _key_name) {
    std::string reassembled_data;
    std::string partial_data;
    for(int partial_data_idx = 0; !(partial_data = decoded_key_value_map_[_key_name+std::to_string(partial_data_idx)]).empty(); partial_data_idx++) {
        reassembled_data.insert(reassembled_data.end(), partial_data.begin(), partial_data.end());
    }
    return reassembled_data;
}
