cmake_minimum_required(VERSION 3.5)

set(SUBPROJ_NAME fprog)

project(${SUBPROJ_NAME} C)

#######################################################################
## Подключение файло исходных кодов и заголовков

set(SOURCES			"${BRD_PATH}/source/flash_programmer.c")

set(INCLUDE			"${BRD_PATH}/include")

set(CORE_INCLUDE	"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include")

set(NXP_DEV_INCLUDE	"${CMAKE_CURRENT_SOURCE_DIR}/base/device/nxp/lpc546xx/include")

set(NXP_DRV_INCLUDE	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/nxp/lpc546xx/include")

set(NXP_DRV_SOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/base/driver/nxp/lpc546xx/source")
file(GLOB			NXP_DRV_SOURCES "${NXP_DRV_SOURCES_PATH}/*.c")
#set(NXP_DRV_SOURCES	"${NXP_DRV_SOURCES}"
#					"${NXP_DRV_SOURCES_PATH}/fsl_clock.c"
#					"${NXP_DRV_SOURCES_PATH}/fsl_gpio.c"
#					"${NXP_DRV_SOURCES_PATH}/fsl_reset.c"
#					"${NXP_DRV_SOURCES_PATH}/fsl_power.c")

#######################################################################
## Настройка параметров сбоки и компоновки

set(C_FLAGS			"-mcpu=cortex-m4;;-Werror")

set(FPROG_LINKER_FLAGS	"-T ${BRD_PATH}/fprog_lpc54618j512.ld"
						"-mcpu=cortex-m4"
						"-Wl,--gc-sections"
						"-nostdlib"
	)
					
set(FPROG_LINKER_LIBS 	"-lc"
						"-lm"
						"-lnosys"
						"-lpower_softabi -L${BRD_PATH}"
	)

add_executable(${SUBPROJ_NAME} ${SOURCES} ${NXP_DRV_SOURCES})

target_include_directories(${SUBPROJ_NAME} PRIVATE ${INCLUDE} ${NXP_DRV_INCLUDE} ${NXP_DEV_INCLUDE} ${CORE_INCLUDE})

target_compile_options(${SUBPROJ_NAME} PRIVATE ${C_FLAGS})

target_link_libraries(${SUBPROJ_NAME} PRIVATE ${FPROG_LINKER_LIBS} ${FPROG_LINKER_FLAGS})

target_compile_definitions(${SUBPROJ_NAME} PRIVATE "CPU_LPC54618J512ET180")

add_custom_command(
        TARGET ${SUBPROJ_NAME} POST_BUILD
        COMMAND arm-none-eabi-objcopy -O binary
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.elf
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.bin)
