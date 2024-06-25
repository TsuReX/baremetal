#!/bin/sh

while inotifywait -rqe modify ${PWD}/../boards/imx8m/source; do
    echo "---> New build"
    cmake --build .
    sleep 2
    echo ""
done