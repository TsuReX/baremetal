cmake_minimum_required(VERSION 3.10)

set(PROJ_NAME debug-dev)

# Предотвращение проверки работоспособности компилятора
# (необходимо для избежания проблемы при кросс-сборке)

set(CMAKE_C_COMPILER_WORKS 1)

# Указание используемых в  проекте языков

project(${PROJ_NAME} C ASM)

# Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS	"-mcpu=cortex-m4 -ggdb -g -flto -Werror")
set(CMAKE_ASM_FLAGS	"-mcpu=cortex-m4 -ggdb -g -flto -Werror")
set(CMAKE_EXECUTABLE_SUFFIX .elf )
set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/flash.ld \
							-mcpu=cortex-m4 -specs=nano.specs \
							-flto -Wl,--gc-sections")

# Подключение основных исходных кодов

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

# Подключение исходных кодов драйверов STM32

set(MCU_MAJOR_MODEL "STM32F3xx")
set(MCU_MINOR_MODEL "STM32F303x8")
set(DRIVER_PATH "STM32Cube_FW_F3_V1.11.0/Drivers")
set(DRIVER_PATH_SRC "${DRIVER_PATH}/${MCU_MAJOR_MODEL}_HAL_Driver/Src/")

string(TOLOWER ${MCU_MAJOR_MODEL} LC_MCU_MAJOR_MODEL)

set(DRIVER_C_SRC
					"${DRIVER_PATH_SRC}/${LC_MCU_MAJOR_MODEL}_ll_utils.c"
					"${DRIVER_PATH_SRC}/${LC_MCU_MAJOR_MODEL}_ll_rcc.c"
					#"${DRIVER_PATH_SRC}/${LC_MCU_MAJOR_MODEL}_ll_gpio.c"
					#"${DRIVER_PATH_SRC}/${LC_MCU_MAJOR_MODEL}_ll_i2c.c"
					#"${DRIVER_PATH_SRC}/${LC_MCU_MAJOR_MODEL}_ll_usart.c"
					)

include_directories("${DRIVER_PATH}/${MCU_MAJOR_MODEL}_HAL_Driver/Inc")
include_directories("${DRIVER_PATH}/CMSIS/Device/ST/${MCU_MAJOR_MODEL}/Include")
include_directories("${DRIVER_PATH}/CMSIS/Core/Include")

add_definitions("-DSTM32F303x8")
add_definitions("-DUSE_FULL_LL_DRIVER")

# Подключение дополнительных библиотек (git-модулей)

add_subdirectory("ringbuf")

# Сборка и компоновка

add_executable(${PROJ_NAME} ${ASM_SRC} ${C_SRC} ${DRIVER_C_SRC})
target_link_libraries(${PROJ_NAME} -lc -lm -lnosys ringbuf)
