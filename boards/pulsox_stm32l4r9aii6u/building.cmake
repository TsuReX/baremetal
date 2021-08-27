cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "pulsox_stm32l4r9aii6u")

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
set(BOARD_SRC_PATH 		"${CMAKE_CURRENT_SOURCE_DIR}/boards/pulsox_stm32l4r9aii6u")

set(INCLUDE				"${INCLUDE}"

						"${BOARD_SRC_PATH}/include"

						"${CMAKE_CURRENT_SOURCE_DIR}/include"
				)

set(MAIN_SOURCES		"${MAIN_SOURCES}"

						"${CMAKE_CURRENT_SOURCE_DIR}/source/pulsox_main.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/debug.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/time.c"

						"${BOARD_SRC_PATH}/source/pulsox_stm32l4r9aii6u.c"
						"${BOARD_SRC_PATH}/source/system_stm32l4xx.c"
						"${BOARD_SRC_PATH}/source/init.c"
						"${BOARD_SRC_PATH}/source/console.c"
						"${BOARD_SRC_PATH}/source/scheduler.c"
				)

set(INCLUDE				"${INCLUDE}"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/base/device/st/stm32l4xx/include"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32l4xx/include"
				)

set(DRV_SOURCES_PATH 	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32l4xx/source")

set(MAIN_SOURCES		"${MAIN_SOURCES}"

						"${DRV_SOURCES_PATH}/stm32l4xx_ll_rcc.c"
						"${DRV_SOURCES_PATH}/stm32l4xx_ll_usart.c"
						"${DRV_SOURCES_PATH}/stm32l4xx_ll_utils.c"
						"${DRV_SOURCES_PATH}/stm32l4xx_ll_gpio.c"
						"${DRV_SOURCES_PATH}/stm32l4xx_ll_i2c.c"
				)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS	"${CMAKE_C_FLAGS} -mcpu=cortex-m4 -Wall")

set(CMAKE_ASM_FLAGS	"${CMAKE_ASM_FLAGS} -mcpu=cortex-m4")

set(LINKER_FLAGS	"${LINKER_FLAGS}"

					"-T ${BOARD_SRC_PATH}/_flash.ld"
					"-mcpu=cortex-m4"
					"-specs=nano.specs"
				)

set(LINKER_LIBS		"${LINKER_LIBS}"

					"-lc"
					"-lm"
					"-lnosys"
				)

add_definitions("-DSTM32L4R9xx")
add_definitions("-DUSE_FULL_LL_DRIVER")
add_definitions("-DDBG_OUT")
