openocd -f openocd.cfg -c "init; reset halt;  flash write_image erase ../build/debug-dev.elf; reset halt"
