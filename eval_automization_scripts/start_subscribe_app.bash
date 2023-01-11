#!/bin/bash

printUsage() {
    printf "Usage $0 <current_run_number> <max_runs>\n"
    printf "Example: $0 0\n"
}

MAX_RUNS=$2
if [[ $# -gt 0 ]]; then
    if [[ $1 -lt $((MAX_RUNS-1)) ]]; then
        cd /home/vsomeip/
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
        while [ -n  ]; do
            sleep 1
        done
        ssh vsomeip_server "pkill notify-sample; tmux kill-session -t service-sample"
        ssh vsomeip_server "tmux new-session -d -s service-sample /home/vsomeip/eval_automization_scripts/start_notify_app.bash"
    fi
else
    printUsage
fi