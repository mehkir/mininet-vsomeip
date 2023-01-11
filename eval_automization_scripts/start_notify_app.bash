#!/bin/bash

cd /home/vsomeip/
/usr/bin/cmake --build /home/vsomeip/build --config Debug --target all -- 1>/dev/null
/usr/bin/cmake --build /home/vsomeip/build --config Debug --target examples -- 1>/dev/null
env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-service.json VSOMEIP_APPLICATION_NAME=service-sample /home/vsomeip/build/examples/notify-sample 1>/dev/null &
