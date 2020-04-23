cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "checkboard_lpc54618j512")

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
set(BRD_PATH 			"${CMAKE_CURRENT_SOURCE_DIR}/boards/checkboard_lpc54618j512")

set(MAIN_SOURCES		"${MAIN_SOURCES}"
						"${BRD_PATH}/source/startup_lpc54618.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/checkboard_main.c"
						"${BRD_PATH}/source/config.c"
						"${BRD_PATH}/source/time.c"
						"${BRD_PATH}/source/scheduler.c"
						"${BRD_PATH}/source/console.c"
						"${BRD_PATH}/source/adc.c"
						"${BRD_PATH}/source/freq_meter.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/nmi_handler.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/context.c"
				)

set(MAIN_INCLUDE		"${BRD_PATH}/include"
						"${CMAKE_CURRENT_SOURCE_DIR}/include/"
				)

set(CORE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include")
				
set(DEVICE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/device/nxp/lpc546xx/include")

set(DRIVER_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/nxp/lpc546xx/include")

#file(GLOB DRIVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/base/Driver/NXP/LPC546xx/source/*.c")
set(DRV_SOURCES_PATH 	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/nxp/lpc546xx/source")
set(DRIVER_SOURCES		"${DRV_SOURCES}"
#						"${DRV_SOURCES_PATH}/fsl_clock.c"
#						"${DRV_SOURCES_PATH}/fsl_common.c"
						"${DRV_SOURCES_PATH}/fsl_gpio.c"
						"${DRV_SOURCES_PATH}/fsl_reset.c"
						"${DRV_SOURCES_PATH}/fsl_rit.c"
						"${DRV_SOURCES_PATH}/fsl_power.c"
						"${DRV_SOURCES_PATH}/fsl_usart.c"
						"${DRV_SOURCES_PATH}/fsl_dma.c"
						"${DRV_SOURCES_PATH}/fsl_usart_dma.c"
						"${DRV_SOURCES_PATH}/fsl_flexcomm.c"
						"${DRV_SOURCES_PATH}/fsl_adc.c"
#						"${DRV_SOURCES_PATH}/fsl_sctimer.c"
				)
#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -mcpu=cortex-m4")

set(CMAKE_ASM_FLAGS		"${CMAKE_ASM_FLAGS} -mcpu=cortex-m4")

set(LINKER_FLAGS		"${LINKER_FLAGS}"
						"-T ${BRD_PATH}/flash_lpc54618j512.ld"
						"-mcpu=cortex-m4 -specs=nano.specs"
						"-Wl,--gc-sections"
						"-nostdlib"
				)

set(LINKER_LIBS 		"-lc"
						"-lm"
						"-lnosys"
						"-lpower_softabi -L${BRD_PATH}"
				)

add_definitions("-DCPU_LPC54618J512ET180")

#######################################################################
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME} fprog)

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md

# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BRD_PATH}/oocd_jlink.cfg -f ${BRD_PATH}/lpc546xx.cfg -c \"do flash\")

#######################################################################
# Определение дополнительной цели для выполнения операции отладки
add_custom_target("debug" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором

# Определение команд для цели flash
add_custom_command(	TARGET "debug"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BRD_PATH}/oocd_jlink.cfg -f ${BRD_PATH}/lpc546xx.cfg -c \"do debug\")


include(${BRD_PATH}/fprog_building.cmake)
					