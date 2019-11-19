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

set(C_SRC	"src/main.c"
			"src/config.c"
			#"src/i2c.c"
			"src/usart1.c"
			"src/console.c"
			"src/stm32f3xx_it.c"
			"src/system_stm32f3xx.c"
			)

set(ASM_SRC	"src/startup_stm32f303x8.s" )

include_directories("include")

add_definitions("-DSTM32F303x8")
add_definitions("-DUSE_FULL_LL_DRIVER")

set(MCU_MODEL "STM32F3xx")
set(DRIVER_PATH "STM32Cube_FW_F3_V1.11.0/Drivers")
set(DRIVER_SRC_PATH "${DRIVER_PATH}/${MCU_MODEL}_HAL_Driver/Src/")

string(TOLOWER ${MCU_MODEL} LC_MCU_MODEL)

set(DRIVER_C_SRC
					"${DRIVER_SRC_PATH}/${LC_MCU_MODEL}_ll_utils.c"
					"${DRIVER_SRC_PATH}/${LC_MCU_MODEL}_ll_rcc.c"
					#"${DRIVER_SRC_PATH}/${LC_MCU_MODEL}_ll_gpio.c"
					#"${DRIVER_SRC_PATH}/${LC_MCU_MODEL}_ll_i2c.c"
					#"${DRIVER_SRC_PATH}/${LC_MCU_MODEL}_ll_usart.c"
					)

include_directories("${DRIVER_PATH}/${MCU_MODEL}_HAL_Driver/Inc")
include_directories("${DRIVER_PATH}/CMSIS/Device/ST/${MCU_MODEL}/Include")
include_directories("${DRIVER_PATH}/CMSIS/Core/Include")

add_subdirectory("ringbuf")

add_executable(${PROJ_NAME} ${ASM_SRC} ${C_SRC} ${DRIVER_C_SRC})

target_link_libraries(${PROJ_NAME} -lc -lm -lnosys ringbuf)
