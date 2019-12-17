clear ; rm -rf ./* ; cmake .. -DCMAKE_C_COMPILER=arm-none-eabi-gcc -DDRIVER_PATH=../STM32Cube_FW_F3_V1.11.0/Drivers -DMCU_MAJOR_MODEL=STM32F3xx -DMCU_MINOR_MODEL=STM32F303x8 -DUSE_FULL_LL_DRIVER=y; make

clear ; rm -rf ./* ; cmake .. -DCMAKE_C_COMPILER=arm-none-eabi-gcc -DDRIVER_PATH=../STM32Cube_FW_F3_V1.11.0/Drivers -DBOARD_TYPE=nucleo_board_f303k8t6; make