#ifndef VSOMEIP_PAYLOAD_KEY_VALUE_DECODER_HPP
#define VSOMEIP_PAYLOAD_KEY_VALUE_DECODER_HPP

#include <string>
#include <map>

class payload_key_value_decoder {
    public:
        payload_key_value_decoder();
        ~payload_key_value_decoder();
        void decode(std::string& _encoded_payload_data);
        std::string get_item(const std::string& _key_name);
    private:
        std::map<std::string, std::string> decoded_key_value_map_;
};

#endif /* VSOMEIP_PAYLOAD_KEY_VALUE_DECODER_HPP */