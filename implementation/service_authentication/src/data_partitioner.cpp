#include "../include/data_partitioner.hpp"
#include <sstream>
#include <boost/algorithm/string.hpp>

#define CERTKEY "cert"

namespace vsomeip_v3 {
namespace sd {

data_partitioner::data_partitioner() {
}

data_partitioner::~data_partitioner() {
}

std::string data_partitioner::convert_to_comma_separated_int_string(std::vector<unsigned char> _data) {
    std::stringstream sstream;
    for (auto it = _data.begin(); it != _data.end(); it++ ) {
        sstream << (uint32_t) *it;
        if ( (it+1) != _data.end() ) {
            sstream << ",";
        }
    }
    std::string msen;
    return sstream.str();
}

std::vector<unsigned char> data_partitioner::convert_comma_separated_string_to_unsigned_char_vector(std::string _comma_separated_string) {
    std::vector<std::string> string_vector;
    boost::split(string_vector, _comma_separated_string, boost::is_any_of(","));
    std::vector<unsigned char> uchar_vector;
    for (auto string_element : string_vector) {
        uchar_vector.push_back((unsigned char) std::atoi(string_element.c_str()));
    }
    return uchar_vector;
}

}
}