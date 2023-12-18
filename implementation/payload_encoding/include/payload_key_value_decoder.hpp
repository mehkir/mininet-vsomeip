#ifndef VSOMEIP_PAYLOAD_KEY_VALUE_DECODER_HPP
#define VSOMEIP_PAYLOAD_KEY_VALUE_DECODER_HPP

#include <string>
#include <vector>
#include <map>

class payload_key_value_decoder {
    public:
        payload_key_value_decoder();
        ~payload_key_value_decoder();
        void decode(std::vector<unsigned char>& _encoded_payload_vector) const;
        std::vector<unsigned char> get_item(const std::string& _key_name) const;
    private:
        mutable std::map<std::string, std::vector<unsigned char>> decoded_key_value_map_;
};

#endif /* VSOMEIP_PAYLOAD_KEY_VALUE_DECODER_HPP */