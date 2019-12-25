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
			"src/console.c"
			"src/scheduler.c"
			"src/stm32f3xx_it.c"
			"src/system_stm32f3xx.c"
			)

set(ASM_SRC	"src/startup_stm32f303x8.s" )

include_directories("include")

# Проверка параметров сборки

set(BOARD_TYPE_STATUS "NOT_SET")

if (DEFINED BOARD_TYPE)
	file(GLOB BOARD_SCRIPTS "boards/*.cmake")
endif()

if (DEFINED BOARD_SCRIPTS)
	foreach(SCRIPT ${BOARD_SCRIPTS})
		include(${SCRIPT})
		if(${BOARD_TYPE_STATUS} STREQUAL "SET")
			break()
		endif()
	endforeach()
endif()

if (DEFINED BOARD_TYPE AND ${BOARD_TYPE_STATUS} STREQUAL "NOT_SET")
	message(FATAL_ERROR " Unknown BOARD_TYPE was specified.\n"
	" Specifiy correct BOARD_TYPE or specify parameters manualy.")
endif()

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")

if (NOT DEFINED DRIVER_PATH)
	message(FATAL_ERROR " DRIVER_PATH must be defined.\n"
	" Ex. -DDRIVER_PATH=/home/user/STM32Cube_FW_F3_V1.11.0/Drivers\n")
endif ()

message(STATUS "DRIVER_PATH: ${DRIVER_PATH}")

if (NOT DEFINED MCU_MAJOR_MODEL)
	message(FATAL_ERROR " MCU_MAJOR_MODEL must be defined.\n"
	" Ex. -DMCU_MAJOR_MODEL=STM32F3xx")

endif ()

message(STATUS "MCU_MAJOR_MODEL: ${MCU_MAJOR_MODEL}")

if (NOT DEFINED MCU_MINOR_MODEL)
	message(FATAL_ERROR " MCU_MINOR_MODEL must be defined.\n"
	" Ex. -DMCU_MINOR_MODEL=STM32F303x8")
endif ()

message(STATUS "MCU_MINOR_MODEL: ${MCU_MINOR_MODEL}")

add_definitions("-D${MCU_MINOR_MODEL}")

if (DEFINED USE_FULL_LL_DRIVER AND USE_FULL_LL_DRIVER STREQUAL "y")
	add_definitions("-DUSE_FULL_LL_DRIVER")
	message(STATUS "Full driver is used")
endif ()

# Подключение исходных кодов драйверов STM32

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

# Подключение дополнительных библиотек (git-модулей)

add_subdirectory("ringbuf")
include_directories("ringbuf/include")

# Определение дополнительной цели для выполнения операции прошивки

add_custom_target("flash")
add_custom_command(TARGET "flash" POST_BUILD COMMAND date)

# Сборка и компоновка

add_executable(${PROJ_NAME} ${ASM_SRC} ${C_SRC} ${DRIVER_C_SRC})
target_link_libraries(${PROJ_NAME} -lc -lm -lnosys ringbuf)
