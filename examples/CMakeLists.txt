# Copyright (C) 2015-2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required (VERSION 2.8)

set(EXAMPLE_CONFIG_FILES
    "../config/vsomeip.json"
    "../config/vsomeip-local.json"
    "../config/vsomeip-tcp-client.json"
    "../config/vsomeip-tcp-service.json"
    "../config/vsomeip-udp-client.json"
    "../config/vsomeip-udp-service.json"
)

find_package (vsomeip3 REQUIRED)
find_package(Boost COMPONENTS system thread log REQUIRED)

include_directories (
        ${Boost_INCLUDE_DIR}
        ${VSOMEIP_INCLUDE_DIRS}
)

list(APPEND VSOMEIP_LIBS ${VSOMEIP_NAME}-cfg ${VSOMEIP_NAME}-e2e ${VSOMEIP_NAME}-sd)

# Examples
add_executable(request-sample request-sample.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(request-sample ${VSOMEIP_NAME} ${VSOMEIP_LIBS} ${Boost_LIBRARIES} ${DL_LIBRARY})

add_executable(response-sample response-sample.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(response-sample ${VSOMEIP_NAME} ${VSOMEIP_LIBS} ${Boost_LIBRARIES} ${DL_LIBRARY})

add_executable(subscribe-sample subscribe-sample.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(subscribe-sample ${VSOMEIP_NAME} ${VSOMEIP_LIBS} ${Boost_LIBRARIES} ${DL_LIBRARY})

add_executable(notify-sample notify-sample.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(notify-sample ${VSOMEIP_NAME} ${VSOMEIP_LIBS} ${Boost_LIBRARIES} ${DL_LIBRARY})

add_dependencies(examples request-sample response-sample subscribe-sample notify-sample)

install (
    TARGETS request-sample response-sample subscribe-sample notify-sample
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
)

###################################################################################################
