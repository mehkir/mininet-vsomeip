#!/usr/bin/bash

$(which cmake) -B /home/vsomeip/build -S /home/vsomeip
$(which cmake) --build /home/vsomeip/build --config Release --target all -- -j$(nproc)
$(which cmake) --build /home/vsomeip/build --config Release --target examples -- -j$(nproc)
$(which cmake) --build /home/vsomeip/build --config Release --target statistics-writer -- -j$(nproc)

# Execution commands for service-sample
# env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-service.json VSOMEIP_APPLICATION_NAME=service-sample /home/vsomeip/build/examples/notify-sample
# Execution commands for client-sample
# env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-client.json VSOMEIP_APPLICATION_NAME=client-sample /home/vsomeip/build/examples/subscribe-sample
