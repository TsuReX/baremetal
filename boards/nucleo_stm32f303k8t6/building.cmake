cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "nucleo_stm32f303k8t6")

# Если тип платформы не установлен, то завершаем выполнение скрипта
if (NOT DEFINED BOARD_TYPE)
	return()
endif ()

# Если заданый тип платформы не соответствует типу платформы,
# для которого используется текущий скрипт,
# то завершаем выполнение текущего скрипта
if (NOT ${BOARD_TYPE} STREQUAL ${CURRENT_BOARD_TYPE})
	return()	
endif ()

set(BOARD_TYPE_STATUS "SET")

#######################################################################
## Подключение файло исходных кодов и заголовков
set(MAIN_ASM_SOURCES    "${CMAKE_CURRENT_SOURCE_DIR}/boards/nucleo_stm32f303k8t6/source/startup_stm32f303x8.s" )

set(MAIN_INCLUDE	${MAIN_INCLUDE}	"${CMAKE_CURRENT_SOURCE_DIR}/boards/nucleo_stm32f303k8t6/include")

set(BOARD_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/boards/nucleo_stm32f303k8t6/source/")

set(MAIN_SOURCES	"${MAIN_SOURCES}"
					"${BOARD_SRC_PATH}/stm32f3xx_it.c"
					"${BOARD_SRC_PATH}/system_stm32f3xx.c"
					"${BOARD_SRC_PATH}/init.c"
					"${BOARD_SRC_PATH}/console.c"
					"${BOARD_SRC_PATH}/scheduler.c"
					"${BOARD_SRC_PATH}/i2c.c"
					"${BOARD_SRC_PATH}/gpio.c"
					"${BOARD_SRC_PATH}/pwm.c"
					)
					
set(DEVICE_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/base/Device/ST/STM32F3xx/Include")

set(DRIVER_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/base/Driver/ST/stm32f3xx/include")

file(GLOB DRIVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/base/Driver/ST/stm32f3xx/source/*_ll_*.c")

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS	"${CMAKE_C_FLAGS} -mcpu=cortex-m4")
	
set(CMAKE_ASM_FLAGS	"${CMAKE_ASM_FLAGS} -mcpu=cortex-m4")
	
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}	-T ${CMAKE_CURRENT_SOURCE_DIR}/boards/nucleo_stm32f303k8t6/flash.ld \
														-mcpu=cortex-m4 -specs=nano.specs \
														-Wl,--gc-sections")
								
add_definitions("-DSTM32F303x8")
add_definitions("-DUSE_FULL_LL_DRIVER")

#######################################################################
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md
set(OOCD_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/boards/nucleo_stm32f303k8t6/oocd_stlinkv2-1.cfg")

# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${OOCD_CONFIG} -c \"init$<SEMICOLON>
							reset halt$<SEMICOLON>
							flash write_image erase ${CMAKE_BINARY_DIR}/${PROJ_NAME}${CMAKE_EXECUTABLE_SUFFIX}$<SEMICOLON>
							reset$<SEMICOLON>
							exit\")
