#!/bin/bash

compile() {
    echo "Compiling notifiy-app..."
    cd /home/vsomeip/
    /usr/bin/cmake --build /home/vsomeip/build --config Release --target all --
    /usr/bin/cmake --build /home/vsomeip/build --config Release --target examples --
    echo "done."
}

start() {
    cd /home/vsomeip/
    env VSOMEIP_CONFIGURATION=/home/vsomeip/config/vsomeip-udp-service.json VSOMEIP_APPLICATION_NAME=service-sample /home/vsomeip/build/examples/notify-sample &
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