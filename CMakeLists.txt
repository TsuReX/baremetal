cmake_minimum_required(VERSION 3.10)

set(PROJ_NAME debug-dev)

set(CMAKE_C_COMPILER_WORKS 1)

project(${PROJ_NAME} C ASM)

set(CMAKE_C_FLAGS	"-mcpu=cortex-m4 -ggdb -g -flto -Werror")

set(CMAKE_ASM_FLAGS	"-mcpu=cortex-m4 -ggdb -g -flto -Werror")

set(CMAKE_EXECUTABLE_SUFFIX .elf )

set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/STM32F303K8Tx_FLASH.ld \
							-mcpu=cortex-m4 -specs=nano.specs \
							-flto -Wl,--gc-sections")

set(C_STM_SRC_PATH "STM32Cube_FW_F3_V1.11.0/Drivers/STM32F3xx_HAL_Driver/Src/")

set(C_SRC	"src/main.c"
			"src/config.c"
			"src/usart1.c"
			"src/console.c"
			"src/stm32f3xx_it.c"
			"src/system_stm32f3xx.c"
			"${C_STM_SRC_PATH}/stm32f3xx_ll_utils.c"
			"${C_STM_SRC_PATH}/stm32f3xx_ll_gpio.c"
			"${C_STM_SRC_PATH}/stm32f3xx_ll_rcc.c"
			"${C_STM_SRC_PATH}/stm32f3xx_ll_i2c.c"
			"${C_STM_SRC_PATH}/stm32f3xx_ll_usart.c")

set(ASM_SRC	"src/startup_stm32f303x8.s" )

include_directories("include")

include_directories("STM32Cube_FW_F3_V1.11.0/Drivers/STM32F3xx_HAL_Driver/Inc")

include_directories("STM32Cube_FW_F3_V1.11.0/Drivers/CMSIS/Device/ST/STM32F3xx/Include")

include_directories("STM32Cube_FW_F3_V1.11.0/Drivers/CMSIS/Core/Include")

add_definitions("-DSTM32F303x8")

add_definitions("-DUSE_FULL_LL_DRIVER")

add_executable(${PROJ_NAME} ${ASM_SRC} ${C_SRC})

target_link_libraries(${PROJ_NAME} -lc -lm -lnosys)
