cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "depo_baikalbmc")

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
set(BOARD_SRC_PATH		"${CMAKE_CURRENT_SOURCE_DIR}/boards/depo_baikalbmc/")

set(MAIN_INCLUDE		"${MAIN_INCLUDE}"
						"${CMAKE_CURRENT_SOURCE_DIR}/boards/depo_baikalbmc/include"
						"${CMAKE_CURRENT_SOURCE_DIR}/include"
						"${BOARD_SRC_PATH}/usblib/include"
				)

set(MAIN_SOURCES		"${MAIN_SOURCES}"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/depo_baikalbmc.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/debug.c"
						"${BOARD_SRC_PATH}/source/startup_stm32f407zgt6.c"
						"${BOARD_SRC_PATH}/source/init.c"
						"${BOARD_SRC_PATH}/source/platform.c"
						"${BOARD_SRC_PATH}/source/console.c"
						"${BOARD_SRC_PATH}/source/system_stm32f4xx.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/time.c"

				)

set(CORE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include")
		
set(DEVICE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/device/st/stm32f4xx/include")

set(DRIVER_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f4xx/include")

set(DRV_SOURCES_PATH 	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f4xx/source")

set(DRIVER_SOURCES 		"${DRV_SOURCES}"
#						"${DRV_SOURCES_PATH}/stm32f4xx_ll_i2c.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_ll_gpio.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_ll_usart.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_ll_rcc.c"
#						"${DRV_SOURCES_PATH}/stm32f4xx_ll_bus.c"
#						"${DRV_SOURCES_PATH}/stm32f4xx_ll_tim.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_ll_utils.c"
				)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -mcpu=cortex-m4 -Wall -Werror")
	
set(CMAKE_ASM_FLAGS		"${CMAKE_ASM_FLAGS} -mcpu=cortex-m4")
	
set(LINKER_FLAGS		"${LINKER_FLAGS}"
						"-T ${CMAKE_CURRENT_SOURCE_DIR}/boards/depo_baikalbmc/flash.ld"
						"-mcpu=cortex-m4"
						"-specs=nano.specs"
#						"-Wl,--gc-sections"
#						"-nostdlib"
				)

set(LINKER_LIBS			"-lc"
						"-lm"
						"-lnosys"
				)
		
add_definitions("-DSTM32F407xx")
add_definitions("-DUSE_FULL_LL_DRIVER")
add_definitions("-DDBG_OUT")

#######################################################################
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md

if (NOT DEFINED PROGRAMMER)
	set(PROG_SCRIPT	"oocd_stlinkv2.cfg")
else()
	if (${PROGRAMMER} STREQUAL "ST2")
		set(PROG_SCRIPT	"oocd_stlinkv2.cfg")
	elseif(${PROGRAMMER} STREQUAL "JLINK")
		set(PROG_SCRIPT	"oocd_jlink.cfg")
	else()
		return()
	endif ()
endif ()

# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BOARD_SRC_PATH}/${PROG_SCRIPT} -c \"do flash\")

#######################################################################
# Определение дополнительной цели для выполнения операции отладки
add_custom_target("debug" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором

# Определение команд для цели flash
add_custom_command(	TARGET "debug"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BOARD_SRC_PATH}/${PROG_SCRIPT} -c \"do debug\")
							
