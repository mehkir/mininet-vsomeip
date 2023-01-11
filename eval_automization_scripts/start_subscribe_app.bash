#!/bin/bash
RUN_NUM=0
MAX_RUNS=20
FILE_NUM=0

start_app() {
    cd /home/vsomeip/
    /usr/bin/cmake --build /home/vsomeip/build --config Debug --target all -- 1>/dev/null
    /usr/bin/cmake --build /home/vsomeip/build --config Debug --target examples -- 1>/dev/null
    env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-client.json VSOMEIP_APPLICATION_NAME=client-sample /home/vsomeip/build/examples/subscribe-sample 1>/dev/null &
    while [ -z $(pgrep subscribe-sampl) ]; do
        sleep 1
    done
}

ssh vsomeip_server "pkill start_noti; pkill notify-samp;"
pkill subscribe-sampl
while [[ $RUN_NUM -lt $MAX_RUNS ]]; do
    while [ -f "/home/vsomeip/timestamp_results/timepoints-#${FILE_NUM}.csv" ]; do
        FILE_NUM=$((FILE_NUM+1))
    done
    ssh vsomeip_server "/home/vsomeip/eval_automization_scripts/start_notify_app.bash" 1>/dev/null &
    while [ -z $(ssh vsomeip_server "pgrep notify-samp") ]; do
        sleep 1
    done
    #echo "vsomeip server is started"
    sleep 5 # give server some time to initialize (maybe choose a smaller value)
    start_app
    #echo "vsomeip client is started"
    while [ ! -f "/home/vsomeip/timestamp_results/timepoints-#${FILE_NUM}.csv" ]; do
        sleep 1
    done
    #echo "result file on client is written"
    while [ -z $(ssh vsomeip_server "ls /home/vsomeip/timestamp_results/timepoints-#${FILE_NUM}.csv 2>/dev/null") ]; do
        sleep 1
    done
    #echo "result file on server is written"
    echo "RUN $RUN_NUM finished."
    RUN_NUM=$((RUN_NUM+1))
    pkill subscribe-sampl
    ssh vsomeip_server "pkill start_noti; pkill notify-samp;"
done