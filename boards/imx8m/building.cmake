cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "imx8m")

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
set(BOARD_SRC_PATH	"${CMAKE_CURRENT_SOURCE_DIR}/boards/imx8m")
					
set(INCLUDE		"${INCLUDE}"
				
				"${CMAKE_CURRENT_SOURCE_DIR}/include"
				
				"${BOARD_SRC_PATH}/include"
			)

set(MAIN_SOURCES		"${MAIN_SOURCES}"
				
				"${BOARD_SRC_PATH}/source/startup.S"
				"${BOARD_SRC_PATH}/source/cache.S"
			)

set(MAIN_SOURCES		"${MAIN_SOURCES}"

#				"${BOARD_SRC_PATH}/source/startup_tm4c1233h6pm.c"
#				"${BOARD_SRC_PATH}/source/init.c"
#				"${BOARD_SRC_PATH}/source/platform.c"
#				"${BOARD_SRC_PATH}/source/console.c"

#				"${CMAKE_CURRENT_SOURCE_DIR}/source/time.c"
#				"${CMAKE_CURRENT_SOURCE_DIR}/source/debug.c"
#				"${CMAKE_CURRENT_SOURCE_DIR}/source/stellaris_main.c"
			)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS	" -mcpu=cortex-a57 -g -ggdb")
	
set(CMAKE_ASM_FLAGS	"-mcpu=cortex-a57 -g -ggdb")
	
set(LINKER_FLAGS	"${LINKER_FLAGS}"
					
					"-T ${BOARD_SRC_PATH}/flash_imx8m.ld"
					"-mcpu=cortex-a57"
					"-nostdlib"
				)

set(LINKER_LIBS 	"${LINKER_LIBS}"
					
					"-lc"
					"-lm"
					"-lnosys"
				)
					
add_definitions("-DIMX8M")
add_definitions("-DDBG_OUT")
