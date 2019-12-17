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