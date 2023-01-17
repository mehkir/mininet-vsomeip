#!/bin/bash

compile() {
    printf "Compiling notifiy-app..."
    cd /home/vsomeip/
    /usr/bin/cmake --build /home/vsomeip/build --config Debug --target all -- 1>/dev/null
    /usr/bin/cmake --build /home/vsomeip/build --config Debug --target examples -- 1>/dev/null
    printf " done.\n"
}

start() {
    cd /home/vsomeip/
    env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-service.json VSOMEIP_APPLICATION_NAME=service-sample /home/vsomeip/build/examples/notify-sample 1>/dev/null &
}

if [ $# -gt 0 ]; then
    case $1 in
        "compile")
            compile
            ;;
        "start")
            start
            ;;
        *)
            echo "Not enough arguments." 1>&2
            ;;
    esac
else
    echo "Not enough arguments." 1>&2
fi