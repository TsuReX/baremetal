cmake_minimum_required(VERSION 3.5)

set(PROJ_NAME flash_programmer)

project(${PROJ_NAME} C)

#######################################################################
## Подключение файло исходных кодов и заголовков

set(SOURCES			"${CMAKE_CURRENT_SOURCE_DIR}/flash_programmer.c")

set(INCLUDE			"${INCLUDE}	${CMAKE_CURRENT_SOURCE_DIR}")
				
set(INCLUDE			"${INCLUDE}	${CMAKE_CURRENT_SOURCE_DIR}/../../../base/Device/NXP/LPC546xx/Include")

set(INCLUDE			"${INCLUDE}	${CMAKE_CURRENT_SOURCE_DIR}/../../../base/Driver/NXP/LPC546xx/include")

file(GLOB			DRV_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../../../base/Driver/NXP/LPC546xx/source/*.c")

#######################################################################
## Настройка параметров сбоки и компоновки

set(C_FLAGS			"-mcpu=cortex-m4;-ggdb;-g;-Werror")

set(LINKER_FLAGS	"-T ${CMAKE_CURRENT_SOURCE_DIR}/flash_programmer_lpc54618j512.ld"
					"-mcpu=cortex-m4"
					"-Wl,--gc-sections")
					
add_executable(${PROJ_NAME} ${SOURCES} ${DRV_SOURCES})

target_include_directories(${PROJ_NAME} PRIVATE "${INCLUDE}")

target_compile_options(${PROJ_NAME} PRIVATE "${C_FLAGS}")

target_link_libraries(${PROJ_NAME} PRIVATE "-lc -lm -lnosys ${LINKER_FLAGS}")
