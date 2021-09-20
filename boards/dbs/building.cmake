cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "dbs")

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
set(BOARD_SRC_PATH		"${CMAKE_CURRENT_SOURCE_DIR}/boards/dbs")

set(INCLUDE				"${INCLUDE}"
						
						"${BOARD_SRC_PATH}/include"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/include"
						
						"${BOARD_SRC_PATH}/usb_device"
						
						"${BOARD_SRC_PATH}/usb_device_library/class"
						
						"${BOARD_SRC_PATH}/usb_device_library/core"
				)

set(MAIN_SOURCES		"${MAIN_SOURCES}"

						"${CMAKE_CURRENT_SOURCE_DIR}/source/debug.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/time.c"

						"${BOARD_SRC_PATH}/source/startup_stm32f407zgt6.c"
						"${BOARD_SRC_PATH}/source/main.c"
						"${BOARD_SRC_PATH}/source/init.c"
						"${BOARD_SRC_PATH}/source/platform.c"
						"${BOARD_SRC_PATH}/source/console.c"
						"${BOARD_SRC_PATH}/source/system_stm32f4xx.c"

				)

set(INCLUDE				"${INCLUDE}"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/base/device/st/stm32f4xx/include"
						
						"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f4xx/include"
				)

set(DRV_SOURCES_PATH 	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f4xx/source")

set(MAIN_SOURCES		"${MAIN_SOURCES}"

						"${DRV_SOURCES_PATH}/stm32f4xx_ll_gpio.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_ll_usart.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_ll_rcc.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_ll_utils.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_ll_usb.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_ll_i2c.c"
						
						"${DRV_SOURCES_PATH}/stm32f4xx_hal_gpio.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_hal_pcd.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_hal_pcd_ex.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_hal_rcc.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_hal.c"
						"${DRV_SOURCES_PATH}/stm32f4xx_hal_cortex.c"
						
						"${BOARD_SRC_PATH}/usb_device/usb_device.c"
						"${BOARD_SRC_PATH}/usb_device/usbd_desc.c"
						"${BOARD_SRC_PATH}/usb_device/usbd_cdc_if.c"
						"${BOARD_SRC_PATH}/usb_device/usbd_conf.c"
						
						"${BOARD_SRC_PATH}/usb_device_library/core/usbd_core.c"
						"${BOARD_SRC_PATH}/usb_device_library/core/usbd_ioreq.c"
						"${BOARD_SRC_PATH}/usb_device_library/core/usbd_ctlreq.c"
						
						"${BOARD_SRC_PATH}/usb_device_library/class/usbd_cdc.c"
				)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -mcpu=cortex-m4 -Wall -Werror")
	
set(CMAKE_ASM_FLAGS		"${CMAKE_ASM_FLAGS} -mcpu=cortex-m4")
	
set(LINKER_FLAGS		"${LINKER_FLAGS}"

						"-T ${BOARD_SRC_PATH}/flash.ld"
						"-mcpu=cortex-m4"
						"-specs=nano.specs"
				)

set(LINKER_LIBS			"${LINKER_LIBS}"

						"-lc"
						"-lm"
						"-lnosys"
				)
		
add_definitions("-DSTM32F407xx")
add_definitions("-DUSE_FULL_LL_DRIVER")
add_definitions("-DDBG_OUT")
add_definitions("-DF407VET6")
#add_definitions("-DF407ZGT6")
