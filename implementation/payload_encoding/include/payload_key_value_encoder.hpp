#ifndef VSOMEIP_PAYLOAD_KEY_VALUE_ENCODER_HPP
#define VSOMEIP_PAYLOAD_KEY_VALUE_ENCODER_HPP

#include <string>

class payload_key_value_encoder {
    public:
        payload_key_value_encoder();
        ~payload_key_value_encoder();
        void add_item(const std::string& _key_name, const std::string& _value, std::string& _payload_data) const ;
    private:
};

#endif /* VSOMEIP_PAYLOAD_KEY_VALUE_ENCODER_HPP */