//
// Created by mehkir on 21.09.22.
//

#ifndef VSOMEIP_LOGGER_HPP
#define VSOMEIP_LOGGER_HPP

enum LOG_TYPE {
    NONE,
    DEBUG,
};

#define LOG_LEVEL DEBUG

#define LOG_DEBUG(m) if (LOG_LEVEL==DEBUG) { \
    std::cout<<m<<std::endl;                 \
}

#endif //VSOMEIP_LOGGER_HPP
