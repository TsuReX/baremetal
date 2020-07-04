cmake_minimum_required(VERSION 3.5)

set(SUBPROJ_NAME fprog)

project(${SUBPROJ_NAME} C)

#######################################################################
## Подключение файло исходных кодов и заголовков

set(SOURCES			"${BRD_PATH}/source/flash_programmer.c")

set(INCLUDE			"${BRD_PATH}/include")

set(CORE_INCLUDE	"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include")

set(STM_DEV_INCLUDE	"${CMAKE_CURRENT_SOURCE_DIR}/base/device/st/stm32f1xx/include")

set(STM_DRV_INCLUDE	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f1xx/include")

set(STM_DRV_SOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f1xx/source")

#######################################################################
## Настройка параметров сбоки и компоновки

set(C_FLAGS			"-mcpu=cortex-m3;;-Werror")

set(FPROG_LINKER_FLAGS	"-T ${BRD_PATH}/fprog_mdr32f9.ld"
						"-mcpu=cortex-m3"
						"-Wl,--gc-sections"
						"-nostdlib"
	)
					
set(FPROG_LINKER_LIBS 	"-lc"
						"-lm"
						"-lnosys"
						"-lpower_softabi -L${BRD_PATH}"
	)

add_executable(${SUBPROJ_NAME} ${SOURCES} ${STM_DRV_SOURCES})

target_include_directories(${SUBPROJ_NAME} PRIVATE
											${INCLUDE} 
											${STM_DRV_INCLUDE}
											${STM_DEV_INCLUDE}
											${CORE_INCLUDE}
							)

target_compile_options(${SUBPROJ_NAME} PRIVATE ${C_FLAGS})

target_link_libraries(${SUBPROJ_NAME} PRIVATE ${FPROG_LINKER_LIBS} ${FPROG_LINKER_FLAGS})

target_compile_definitions(${SUBPROJ_NAME} PRIVATE	"STM32F103xB"
													"USE_FULL_LL_DRIVER"
							)
#add_definitions("-DSTM32F103xB")
#add_definitions("-DUSE_FULL_LL_DRIVER")


add_custom_command(
        TARGET ${SUBPROJ_NAME} POST_BUILD
        COMMAND arm-none-eabi-objcopy -O binary
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.elf
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.bin)
                
add_custom_command(
        TARGET ${SUBPROJ_NAME} POST_BUILD
        COMMAND arm-none-eabi-objdump -SD
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.elf >
                ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJ_NAME}.elf.objdump)
