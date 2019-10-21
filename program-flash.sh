openocd -f openocd.cfg -c "init; reset halt; stm32f3x mass_erase 0; program build/topaz-bmc.bin 0x08000000; reset; exit"
