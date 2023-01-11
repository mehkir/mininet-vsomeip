#!/bin/bash
MAX_RUNS=20

printUsage() {
    printf "Usage $0 <first_run_number>\n"
    printf "Example: $0 0\n"
}

if [[ $# -gt 0 ]]; then
    if [[ $1 -lt $((MAX_RUNS-1)) ]]; then
        pkill notify-sample
        /usr/bin/cmake --build /home/vsomeip/build --config Debug --target all --
        /usr/bin/cmake --build /home/vsomeip/build --config Debug --target examples --
        env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-service.json VSOMEIP_APPLICATION_NAME=service-sample /home/vsomeip/build/examples/notify-sample &
        while [ -z $(pgrep notify-sample) ]; do
            sleep 1
        done
        ssh vsomeip_client '/home/vsomeip/eval_automization_scripts/start_subscribe_app.bash' 1>/dev/null &
    fi
else
    printUsage
fi
