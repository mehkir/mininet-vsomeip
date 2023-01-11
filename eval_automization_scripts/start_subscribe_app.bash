#!/bin/bash
FILE_NUM=0
MAX_RUNS=20

start_app() {
    cd /home/vsomeip/
    /usr/bin/cmake --build /home/vsomeip/build --config Debug --target all --
    /usr/bin/cmake --build /home/vsomeip/build --config Debug --target examples --
    env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-client.json VSOMEIP_APPLICATION_NAME=client-sample /home/vsomeip/build/examples/subscribe-sample 1>/dev/null &
    while [ -z $(pgrep subscribe-sampl) ]; do
        sleep 1
    done
}


while [[ $FILE_NUM -lt $((MAX_RUNS-1)) ]]; do
    ssh vsomeip_server "/home/vsomeip/eval_automization_scripts/start_notify_app.bash ${FILE_NUM}" 1>/dev/null &
    while [ -z $(ssh vsomeip_server "pgrep notify-samp") ]; do
        sleep 1
    done
    sleep 5 # maybe choose a smaller value
    start_app
    while [ ! -f "/home/vsomeip/timestamp_results/timepoints #${FILE_NUM}.csv" ]; do
        sleep 1
    done
    while [ -z $(ssh vsomeip_server "ls /home/vsomeip/timestamp_results/timepoints #${FILE_NUM}.csv") ]; do
        sleep 1
    done
    FILE_NUM=$((FILE_NUM+1))
    pkill subscribe-sampl
    ssh vsomeip_server "pkill start_noti; pkill notify-samp;"
done
ssh vsomeip_server "pkill start_noti; pkill notify-samp;"
pkill subscribe-sampl