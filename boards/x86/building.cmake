cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "x86")

project(${PROJ_NAME} C ASM_NASM)

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
set(BOARD_SRC_PATH	"${CMAKE_CURRENT_SOURCE_DIR}/boards/x86")
					
set(INCLUDE		"${INCLUDE}"
				
				"${CMAKE_CURRENT_SOURCE_DIR}/include"
				
				"${BOARD_SRC_PATH}/include"
			)

set(MAIN_SOURCES		"${MAIN_SOURCES}"
				
			"${BOARD_SRC_PATH}/source/startup.nasm"
			"${BOARD_SRC_PATH}/source/protected_mode.nasm"
			"${BOARD_SRC_PATH}/source/car.nasm"
			"${BOARD_SRC_PATH}/source/microcode_update.nasm"
			)

set(MAIN_SOURCES		"${MAIN_SOURCES}"
				"${BOARD_SRC_PATH}/source/main.c"
				"${BOARD_SRC_PATH}/source/io.c"
				"${BOARD_SRC_PATH}/source/uart.c"
				"${BOARD_SRC_PATH}/source/ite8613.c"
				"${BOARD_SRC_PATH}/source/ast2500.c"
			)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS	" -march=i386 -g -ggdb ")
	
set(CMAKE_ASM_NASM_FLAGS	"-g -F dwarf -O0")
	
set(LINKER_FLAGS	"${LINKER_FLAGS}"
					
					"-T ${BOARD_SRC_PATH}/script.ld"
					"-march=i386"
					"-nostdlib"
					"-L ${BOARD_SRC_PATH}"
					"-L ${CMAKE_BINARY_DIR}"
				)

set(LINKER_LIBS 	"${LINKER_LIBS}"
					
					"-lc"
					"-lm"
					"-lnosys"
				)
					
add_definitions("-DX86")
add_definitions("-DDBG_OUT")

if (DEFINED UART_TYPE)
    message("-- UART_TYPE=${UART_TYPE}")
    add_compile_definitions("UART_TYPE=${UART_TYPE}")
else()
    message(SEND_ERROR " UART_TYPE isn't defined. The value can be: LOCAL, ITE8613, AST2500")
endif()

if (DEFINED UART_NUM)
    message("-- UART_NUM=${UART_NUM}")
    add_compile_definitions(UART_NUM=${UART_NUM})
else()
    message(SEND_ERROR "UART_NUM isn't defined. The value can be: 0-(0x3F8), 1-(0x2F8), 2-(0x3E8), 3-(0x2E8)")
endif()
