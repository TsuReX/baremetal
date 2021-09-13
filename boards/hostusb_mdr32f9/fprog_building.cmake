cmake_minimum_required(VERSION 3.5)

set(SUBPROJ_NAME fprog)

project(${SUBPROJ_NAME} C)

#######################################################################
## Подключение файло исходных кодов и заголовков

set(FPROG_SOURCES	"${FPROG_SOURCES}"
					"${BOARD_SRC_PATH}/source/flash_programmer.c")

set(FPROG_INCLUDE	"${FPROG_INCLUDE}"
					"${BOARD_SRC_PATH}/include"
					"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include"
					"${CMAKE_CURRENT_SOURCE_DIR}/base/device/milandr/mdr32f9qx/include"
					"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/milandr/mdr32f9qx/include")

set(MDR_DRV_SOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/base/driver/milandr/mdr32f9qx/source")

set(FPROG_SOURCES	"${FPROG_SOURCES}"
					"${MDR_DRV_SOURCES_PATH}/MDR32F9Qx_eeprom.c"
					"${MDR_DRV_SOURCES_PATH}/MDR32F9Qx_rst_clk.c"
	)
#######################################################################
## Настройка параметров сбоки и компоновки

set(C_FLAGS			"-mcpu=cortex-m3;;-Werror")

set(FPROG_LINKER_FLAGS		"${FPROG_LINKER_FLAGS}"

							"-T ${BOARD_SRC_PATH}/fprog_mdr32f9.ld"
							"-mcpu=cortex-m3"
							"-Wl,--gc-sections"
							"-nostdlib"
						)

set(FPROG_LINKER_LIBS		"${FPROG_LINKER_LIBS}"

							"-lc"
							"-lm"
							"-lnosys"
						)

add_executable(${SUBPROJ_NAME} ${FPROG_SOURCES})

target_include_directories(${SUBPROJ_NAME} PRIVATE ${FPROG_INCLUDE})

target_compile_options(${SUBPROJ_NAME} PRIVATE ${C_FLAGS})

target_link_libraries(${SUBPROJ_NAME} PRIVATE	
												${FPROG_LINKER_LIBS}
												${FPROG_LINKER_FLAGS}
											)

target_compile_definitions(${SUBPROJ_NAME} PRIVATE	
													"USE_MDR1986VE9x"
													"USE_FULL_LL_DRIVER"
											)

add_custom_command(
        TARGET ${SUBPROJ_NAME} POST_BUILD
        COMMAND "${CMAKE_C_COMPILER_PREFIX}objcopy" -O binary
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.elf
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.bin)
                
add_custom_command(
        TARGET ${SUBPROJ_NAME} POST_BUILD
        COMMAND "${CMAKE_C_COMPILER_PREFIX}objdump" -SD
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.elf >
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.elf.objdump)
