cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "depo_mdr32f9")

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
set(BRD_PATH		"${CMAKE_CURRENT_SOURCE_DIR}/boards/depo_mdr32f9/")

set(MAIN_ASM_SOURCES    "${BRD_PATH}/source/startup_stm32f103xb.s")

set(MAIN_INCLUDE		"${MAIN_INCLUDE}"
						"${CMAKE_CURRENT_SOURCE_DIR}/boards/depo_mdr32f9/include"

						"${BRD_PATH}/usb/include"

						"${BRD_PATH}/usblib/include"
				)

set(MAIN_SOURCES		"${MAIN_SOURCES}"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/depo_mdr32f9.c"
						"${BRD_PATH}/source/stm32f1xx_it.c"
						"${BRD_PATH}/source/system_stm32f1xx.c"
						"${BRD_PATH}/source/config.c"
						"${BRD_PATH}/source/console.c"
						"${BRD_PATH}/source/scheduler.c"

#						"${BRD_PATH}/usb/source/usb_desc.c"
#						"${BRD_PATH}/usb/source/usb_endp.c"
#						"${BRD_PATH}/usb/source/usb_istr.c"
#						"${BRD_PATH}/usb/source/usb_prop.c"
#						"${BRD_PATH}/usb/source/usb_pwr.c"

#						"${BRD_PATH}/usblib/source/usb_core.c"
#						"${BRD_PATH}/usblib/source/usb_init.c"
#						"${BRD_PATH}/usblib/source/usb_int.c"
#						"${BRD_PATH}/usblib/source/usb_mem.c"
#						"${BRD_PATH}/usblib/source/usb_regs.c"
#						"${BRD_PATH}/usblib/source/usb_sil.c"
				)

set(CORE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include")
		
set(DEVICE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/device/st/stm32f1xx/include")

set(DRIVER_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f1xx/include")

set(DRV_SOURCES_PATH 	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f1xx/source")

set(DRIVER_SOURCES 		"${DRV_SOURCES}"
						"${DRV_SOURCES_PATH}/stm32f1xx_ll_i2c.c"
						"${DRV_SOURCES_PATH}/stm32f1xx_ll_gpio.c"
						"${DRV_SOURCES_PATH}/stm32f1xx_ll_usart.c"
						"${DRV_SOURCES_PATH}/stm32f1xx_ll_dma.c"
						"${DRV_SOURCES_PATH}/stm32f1xx_ll_tim.c"
						"${DRV_SOURCES_PATH}/stm32f1xx_ll_utils.c"
				)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -mcpu=cortex-m3 -Wall")
	
set(CMAKE_ASM_FLAGS		"${CMAKE_ASM_FLAGS} -mcpu=cortex-m3")
	
set(LINKER_FLAGS		"${LINKER_FLAGS}"
						"-T ${CMAKE_CURRENT_SOURCE_DIR}/boards/depo_mdr32f9/flash_mdr32f9.ld"
						"-mcpu=cortex-m3"
						"-specs=nano.specs"
						"-Wl,--gc-sections"
#						"-nostdlib"
				)

set(LINKER_LIBS			"-lc"
						"-lm"
						"-lnosys"
				)
		
add_definitions("-DSTM32F103xB")
add_definitions("-DUSE_FULL_LL_DRIVER")

#######################################################################
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME} fprog)

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md

# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BRD_PATH}/oocd_jlink.cfg -c \"do flash\")

#######################################################################
# Определение дополнительной цели для выполнения операции отладки
add_custom_target("debug" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором

# Определение команд для цели flash
add_custom_command(	TARGET "debug"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BRD_PATH}/oocd_jlink.cfg -c \"do debug\")
				
include(${BRD_PATH}/fprog_building.cmake)
