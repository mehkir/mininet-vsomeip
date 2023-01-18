#!/bin/bash
RUN_NUM=0
MAX_RUNS=20
FILE_NUM=0

compile() {
    echo "Compiling subscribe-app..."
    cd /home/vsomeip/
    /usr/bin/cmake --build /home/vsomeip/build --config Debug --target all --
    /usr/bin/cmake --build /home/vsomeip/build --config Debug --target examples --
    echo "done."
}

start() {
    cd /home/vsomeip/
    env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-client.json VSOMEIP_APPLICATION_NAME=client-sample /home/vsomeip/build/examples/subscribe-sample &
    while [ -z $(pgrep subscribe-sampl) ]; do
        sleep 1
    done
}

function on_exit() {
    echo "Terminating processes..."
    pkill subscribe-sampl
    ssh vsomeip_server "pkill notify-samp; pkill start_noti;"
    echo "Terminated processes."
    exit 1
}

trap 'on_exit' SIGINT

ssh vsomeip_server "pkill notify-samp; pkill start_noti;"
pkill subscribe-sampl
ssh vsomeip_server "/home/vsomeip/eval_automization_scripts/start_notify_app.bash compile"
compile
while [[ $RUN_NUM -lt $MAX_RUNS ]]; do
    while [ -f "/home/vsomeip/timestamp_results/timepoints-#${FILE_NUM}.csv" ]; do
        FILE_NUM=$((FILE_NUM+1))
    done
    ssh vsomeip_server "/home/vsomeip/eval_automization_scripts/start_notify_app.bash start" &
    while [ -z $(ssh vsomeip_server "pgrep notify-samp") ]; do
        sleep 1
    done
    #echo "vsomeip server is started"
    sleep 5 # give server some time to initialize (maybe choose a smaller value)
    start
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
    ssh vsomeip_server "pkill notify-samp; pkill start_noti;"
done