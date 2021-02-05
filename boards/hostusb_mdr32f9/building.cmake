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
set(BRD_PATH		"${CMAKE_CURRENT_SOURCE_DIR}/boards/hostusb_mdr32f9/")

#set(MAIN_ASM_SOURCES    "${BRD_PATH}/source/startup_MDR32F9Qx.s")

set(MAIN_INCLUDE		"${MAIN_INCLUDE}"
						"${BRD_PATH}/include"
						"${CMAKE_CURRENT_SOURCE_DIR}/include"
				)

set(MAIN_SOURCES		"${MAIN_SOURCES}"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/max3421e.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/kbmsusb.c"
						"${BRD_PATH}/source/startup_mdr32f9qx.c"
#						"${BRD_PATH}/source/system_MDR32F9Qx.c"
						"${BRD_PATH}/source/console.c"
						"${BRD_PATH}/source/spi.c"
						"${BRD_PATH}/source/delay.c"
						"${BRD_PATH}/source/init.c"
						"${BRD_PATH}/source/platform.c"
						"${BRD_PATH}/source/communication.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/hostusb_main.c"
				)

set(CORE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include")
		
set(DEVICE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/device/milandr/mdr32f9qx/include")

set(DRIVER_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/milandr/mdr32f9qx/include")

set(DRV_SOURCES_PATH 	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/milandr/mdr32f9qx/source")

set(DRIVER_SOURCES 		"${DRV_SOURCES}"
						"${DRV_SOURCES_PATH}/MDR32F9Qx_rst_clk.c"
						"${DRV_SOURCES_PATH}/MDR32F9Qx_port.c"
						"${DRV_SOURCES_PATH}/MDR32F9Qx_uart.c"
						"${DRV_SOURCES_PATH}/MDR32F9Qx_ssp.c"
				)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -mcpu=cortex-m3 -Wall")
	
set(CMAKE_ASM_FLAGS		"${CMAKE_ASM_FLAGS} -mcpu=cortex-m3")
	
set(LINKER_FLAGS		"${LINKER_FLAGS}"
						"-T ${CMAKE_CURRENT_SOURCE_DIR}/boards/hostusb_mdr32f9/flash_mdr32f9.ld"
						"-mcpu=cortex-m3"
						"-specs=nano.specs"
						"-Wl,--gc-sections"
						"-nostdlib"
				)

set(LINKER_LIBS			"-lc"
						"-lm"
						"-lnosys"
				)
		
add_definitions("-DUSE_MDR1986VE9x")

#######################################################################
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME} fprog)

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md

# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BRD_PATH}/oocd_jlink.cfg -c \"do flash\")

#######################################################################
# Определение дополнительной цели для выполнения операции отладки
add_custom_target("debug" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором

# Определение команд для цели flash
add_custom_command(	TARGET "debug"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BRD_PATH}/oocd_jlink.cfg -c \"do debug\")
				
include(${BRD_PATH}/fprog_building.cmake)
