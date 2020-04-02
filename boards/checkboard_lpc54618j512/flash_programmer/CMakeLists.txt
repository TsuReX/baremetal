cmake_minimum_required(VERSION 3.5)

set(PROJ_NAME flash_programmer)

project(${PROJ_NAME} C)

#######################################################################
## Подключение файло исходных кодов и заголовков

set(SOURCES			"${CMAKE_CURRENT_SOURCE_DIR}/flash_programmer.c")

set(INCLUDE			"${CMAKE_CURRENT_SOURCE_DIR}")

set(CORE_INCLUDE	"${CMAKE_CURRENT_SOURCE_DIR}/../../../base/core/include")

set(NXP_DEV_INCLUDE	"${CMAKE_CURRENT_SOURCE_DIR}/../../../base/device/nxp/lpc546xx/include")

set(NXP_DRV_INCLUDE	"${CMAKE_CURRENT_SOURCE_DIR}/../../../base/driver/nxp/lpc546xx/include")

set(NXP_DRV_SOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../base/driver/nxp/lpc546xx/source")
#file(GLOB			NXP_DRV_SOURCES "${NXP_DRV_SOURCES_PATH}/*.c")
set(NXP_DRV_SOURCES	"${NXP_DRV_SOURCES}"
					"${NXP_DRV_SOURCES_PATH}/fsl_clock.c"
					"${NXP_DRV_SOURCES_PATH}/fsl_usart.c")

#######################################################################
## Настройка параметров сбоки и компоновки

set(C_FLAGS			"-mcpu=cortex-m4;-s;-Werror")

set(LINKER_FLAGS	"-T ${CMAKE_CURRENT_SOURCE_DIR}/flash_programmer_lpc54618j512.ld"
					"-mcpu=cortex-m4"
					"-Wl,--gc-sections")

add_executable(${PROJ_NAME} ${SOURCES} ${NXP_DRV_SOURCES})

target_include_directories(${PROJ_NAME} PRIVATE ${INCLUDE}  ${NXP_DRV_INCLUDE} ${NXP_DEV_INCLUDE} ${CORE_INCLUDE})

target_compile_options(${PROJ_NAME} PRIVATE ${C_FLAGS})

target_link_libraries(${PROJ_NAME} PRIVATE "-lc -lm -lnosys -nostdlib ${LINKER_FLAGS}")

target_compile_definitions(${PROJ_NAME} PRIVATE "CPU_LPC54618J512ET180")

add_custom_command(
        TARGET ${PROJ_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.elf
                ${CMAKE_CURRENT_BINARY_DIR}/../../../${PROJ_NAME}.elf)

add_custom_command(
        TARGET ${PROJ_NAME} POST_BUILD
        COMMAND arm-none-eabi-objcopy -O binary
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.elf
                ${CMAKE_CURRENT_BINARY_DIR}/../../../${PROJ_NAME}.bin)
