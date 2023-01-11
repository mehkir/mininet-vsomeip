#!/bin/bash

printUsage() {
    printf "Usage $0 <current_run_number> <max_runs>\n"
    printf "Example: $0 0\n"
}

MAX_RUNS=$2
if [[ $# -gt 0 ]]; then
    if [[ $1 -lt $((MAX_RUNS-1)) ]]; then
        pkill subscribe-sample
        /usr/bin/cmake --build /home/vsomeip/build --config Debug --target all --
        /usr/bin/cmake --build /home/vsomeip/build --config Debug --target examples --
        env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-client.json VSOMEIP_APPLICATION_NAME=client-sample /home/vsomeip/build/examples/subscribe-sample &
        while [ -z $(pgrep subscribe-sample) ]; do
            sleep 1
        done
        while [ ! -f "/home/vsomeip/timestamp_results/timepoints #${$1}.csv" ]; do
            sleep 1
        done
        pkill subscribe-sample
        ssh vsomeip_server "cd /home/vsomeip/; /home/vsomeip/eval_automization_scripts/start_notify_app.bash" 1>/dev/null &
    fi
else
    printUsage
fi