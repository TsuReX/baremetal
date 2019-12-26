openocd -f ../openocd.cfg -c "init; reset halt;  flash write_image erase debug-dev.elf; reset; exit"
