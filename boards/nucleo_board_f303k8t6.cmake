cmake_minimum_required(VERSION 3.10)

set(CURRENT_BOARD_TYPE "nucleo_board_f303k8t6")

if (NOT DEFINED BOARD_TYPE)
	return()
endif ()

if (NOT ${BOARD_TYPE} STREQUAL ${CURRENT_BOARD_TYPE})
	return()	
endif ()

set(BOARD_TYPE_STATUS "SET")

message(STATUS "BOARD_TYPE: ${BOARD_TYPE}")

set(MCU_MAJOR_MODEL "STM32F3xx")

set(MCU_MINOR_MODEL "STM32F303x8")

set(USE_FULL_LL_DRIVER "y")

# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
set(OOCD_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/boards/nucleo_board_f303k8t6_stlinkv2-1.cfg")

# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${OOCD_CONFIG} -c \"init$<SEMICOLON>
							reset halt$<SEMICOLON>
							flash write_image erase ${CMAKE_BINARY_DIR}/${PROJ_NAME}${CMAKE_EXECUTABLE_SUFFIX}$<SEMICOLON>
							reset$<SEMICOLON>
							exit\")