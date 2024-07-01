#!/bin/sh

while inotifywait -rqe modify ${PWD}/../boards/x86/source; do
    echo "---> New build: `date`"
    cmake --build .
    sleep 2
    echo ""
done