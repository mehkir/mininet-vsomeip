#ifndef VSOMEIP_PAYLOAD_KEY_VALUE_ENCODER_HPP
#define VSOMEIP_PAYLOAD_KEY_VALUE_ENCODER_HPP

#include <string>
#include <vector>
#include <map>

class payload_key_value_encoder {
    public:
        payload_key_value_encoder();
        ~payload_key_value_encoder();
        void add_item(const std::string& _key_name, const std::vector<unsigned char>& _value) const;
        std::vector<unsigned char> encode() const;
    private:
        mutable std::map<std::string, std::vector<unsigned char>> plain_key_value_map_;
};

#endif /* VSOMEIP_PAYLOAD_KEY_VALUE_ENCODER_HPP */