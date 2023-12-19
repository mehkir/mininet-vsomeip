#include "../include/payload_key_value_decoder.hpp"

payload_key_value_decoder::payload_key_value_decoder() {

}

payload_key_value_decoder::~payload_key_value_decoder() {

}

void payload_key_value_decoder::decode(std::vector<unsigned char>& _encoded_payload_vector) const {
    decoded_key_value_map_.clear();
    for (auto it = _encoded_payload_vector.begin(); it != _encoded_payload_vector.end(); it++ ) {
        size_t item_length = *it;
        it++;
        std::string item(it,it+item_length);
        size_t equal_char_idx = item.find('=');
        std::string key_name = item.substr(0, equal_char_idx);
        std::vector<unsigned char> value(it+equal_char_idx+1,it+item_length);
        decoded_key_value_map_[key_name] = value;
        it+=item_length-1; // -1 since loop increments by one already
    }
}

std::vector<unsigned char> payload_key_value_decoder::get_item(const std::string& _key_name) const {
    std::vector<unsigned char> reassembled_data;
    std::vector<unsigned char> partial_data;
    for(int partial_data_idx = 0; !(partial_data = decoded_key_value_map_[_key_name+std::to_string(partial_data_idx)]).empty(); partial_data_idx++) {
        reassembled_data.insert(reassembled_data.end(), partial_data.begin(), partial_data.end());
    }
    return reassembled_data;
}
