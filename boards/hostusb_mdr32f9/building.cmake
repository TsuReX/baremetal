cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "hostusb_mdr32f9")

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
set(BOARD_SRC_PATH		"${CMAKE_CURRENT_SOURCE_DIR}/boards/hostusb_mdr32f9/")

set(INCLUDE				"${MAIN_INCLUDE}"

						"${BOARD_SRC_PATH}/include"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/include"
				)

set(MAIN_SOURCES		"${MAIN_SOURCES}"

						"${CMAKE_CURRENT_SOURCE_DIR}/source/hostusb_main.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/max3421e.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/kbmsusb.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/systimer.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/time.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/cobs.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/debug.c"
	
						"${BOARD_SRC_PATH}/source/startup_mdr32f9qx.c"
						"${BOARD_SRC_PATH}/source/console.c"
						"${BOARD_SRC_PATH}/source/spi.c"
						"${BOARD_SRC_PATH}/source/init.c"
						"${BOARD_SRC_PATH}/source/platform.c"
						"${BOARD_SRC_PATH}/source/communication.c"
				)

set(INCLUDE				"${INCLUDE}"

						"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/base/device/milandr/mdr32f9qx/include"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/milandr/mdr32f9qx/include"
				)

SET(DRV_SOURCES_PATH	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/milandr/mdr32f9qx/source")

set(MAIN_SOURCES 		"${MAIN_SOURCES}"
						"${DRV_SOURCES_PATH}/MDR32F9Qx_rst_clk.c"
						"${DRV_SOURCES_PATH}/MDR32F9Qx_port.c"
						"${DRV_SOURCES_PATH}/MDR32F9Qx_uart.c"
						"${DRV_SOURCES_PATH}/MDR32F9Qx_ssp.c"
						"${DRV_SOURCES_PATH}/MDR32F9Qx_eeprom.c"
				)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -mcpu=cortex-m3 -Wall")

set(CMAKE_ASM_FLAGS		"${CMAKE_ASM_FLAGS} -mcpu=cortex-m3")

set(LINKER_FLAGS		"${LINKER_FLAGS}"

						"-T ${BOARD_SRC_PATH}/flash_mdr32f9.ld"
						"-mcpu=cortex-m3"
						"-specs=nano.specs"
						"-Wl,--gc-sections"
						"-nostdlib"
				)

set(LINKER_LIBS			"${LINKER_LIBS}"

						"-lc"
						"-lm"
						"-lnosys"
				)

add_definitions("-DUSE_MDR1986VE9x")
#add_definitions("-DDBG_OUT")

include(${BOARD_SRC_PATH}/fprog_building.cmake)
