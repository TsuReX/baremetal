#!/bin/sh
openocd -f $1 -c "init; reset halt;" > /dev/null &  arm-none-eabi-gdb ; echo shutdown | nc 127.0.0.1 4444