#!/usr/bin/bash

$(which cmake) -B /home/vsomeip/build -S /home/vsomeip
$(which cmake) --build /home/vsomeip/build --config Release --target all -- -j$(nproc)
$(which cmake) --build /home/vsomeip/build --config Release --target examples -- -j$(nproc)

# Execution commands for service-sample
# env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-service.json VSOMEIP_APPLICATION_NAME=service-sample ./notify-sample
# Execution commands for client-sample
# env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-client.json VSOMEIP_APPLICATION_NAME=client-sample ./subscribe-sample
