cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "som-carrier")

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
set(BOARD_SRC_PATH		"${CMAKE_CURRENT_SOURCE_DIR}/boards/som-carrier")

set(INCLUDE				"${INCLUDE}"
						
						"${BOARD_SRC_PATH}/include"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/include"
						
				)

set(MAIN_SOURCES		"${MAIN_SOURCES}"

#						"${CMAKE_CURRENT_SOURCE_DIR}/source/debug.c"
#						"${CMAKE_CURRENT_SOURCE_DIR}/source/time.c"

						"${BOARD_SRC_PATH}/source/startup_stm32f407zgt6.c"
						"${BOARD_SRC_PATH}/source/main.c"
						"${BOARD_SRC_PATH}/source/init.c"

				)

set(INCLUDE				"${INCLUDE}"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include"
						
#						"${CMAKE_CURRENT_SOURCE_DIR}/base/device/st/stm32f4xx/include"
						
#						"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f4xx/include"
				)

set(DRV_SOURCES_PATH 	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f4xx/source")

set(MAIN_SOURCES		"${MAIN_SOURCES}"

#						"${DRV_SOURCES_PATH}/stm32f4xx_ll_gpio.c"
				)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -mcpu=cortex-m4 -Wall -Werror")
	
set(CMAKE_ASM_FLAGS		"${CMAKE_ASM_FLAGS} -mcpu=cortex-m4")
	
set(LINKER_FLAGS		"${LINKER_FLAGS}"

						"-T ${BOARD_SRC_PATH}/flash.ld"
						"-mcpu=cortex-m4"
						"-specs=nano.specs"
				)

set(LINKER_LIBS			"${LINKER_LIBS}"

						"-lc"
						"-lm"
						"-lnosys"
				)
		
add_definitions("-DSTM32F407xx")
add_definitions("-DUSE_FULL_LL_DRIVER")
add_definitions("-DDBG_OUT")
add_definitions("-DF407VET6")
#add_definitions("-DF407ZGT6")
