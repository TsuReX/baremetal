cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "devusb_stm32f103c8t6")

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
set(BOARD_SRC_PATH		"${CMAKE_CURRENT_SOURCE_DIR}/boards/devusb_stm32f103c8t6/")

#set(MAIN_ASM_SOURCES    "${BOARD_SRC_PATH}/source/startup_stm32f103xb.s")

set(MAIN_INCLUDE		"${MAIN_INCLUDE}"
						"${CMAKE_CURRENT_SOURCE_DIR}/boards/devusb_stm32f103c8t6/include"
						"${CMAKE_CURRENT_SOURCE_DIR}/include"
						"${BOARD_SRC_PATH}/usblib/include"
				)

set(MAIN_SOURCES		"${MAIN_SOURCES}"
						"${BOARD_SRC_PATH}/source/startup_stm32f103xb.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/devusb_main.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/max3421e.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/kbmsusb.c"
#						"${BOARD_SRC_PATH}/source/stm32f1xx_it.c"
						"${BOARD_SRC_PATH}/source/system_stm32f103xb.c"
						"${BOARD_SRC_PATH}/source/init.c"
						"${BOARD_SRC_PATH}/source/console.c"
						"${BOARD_SRC_PATH}/source/scheduler.c"
						"${BOARD_SRC_PATH}/source/spi.c"
						"${BOARD_SRC_PATH}/source/communication.c"
						"${BOARD_SRC_PATH}/source/utils.c"

						"${BOARD_SRC_PATH}/usblib/source/usb_desc.c"
						"${BOARD_SRC_PATH}/usblib/source/usb_endp.c"
						"${BOARD_SRC_PATH}/usblib/source/usb_irq.c"
						"${BOARD_SRC_PATH}/usblib/source/usb_prop.c"
						"${BOARD_SRC_PATH}/usblib/source/usb_core.c"
						"${BOARD_SRC_PATH}/usblib/source/usb_mem.c"
						"${BOARD_SRC_PATH}/usblib/source/usb_regs.c"
						"${BOARD_SRC_PATH}/usblib/source/usb_sil.c"
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

set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -mcpu=cortex-m3 -Wall -Werror")
	
set(CMAKE_ASM_FLAGS		"${CMAKE_ASM_FLAGS} -mcpu=cortex-m3")
	
set(LINKER_FLAGS		"${LINKER_FLAGS}"
						"-T ${CMAKE_CURRENT_SOURCE_DIR}/boards/devusb_stm32f103c8t6/flash_stm32f103c8t6.ld"
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
#add_custom_target("flash" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md

# Определение команд для цели flash
#add_custom_command(	TARGET "flash"
#					POST_BUILD
#					COMMAND openocd
#					ARGS	-f ${OOCD_CONFIG} -c \"init$<SEMICOLON>
#							reset halt$<SEMICOLON>
#							flash write_image erase ${CMAKE_BINARY_DIR}/${PROJ_NAME}${CMAKE_EXECUTABLE_SUFFIX}$<SEMICOLON>
#							reset$<SEMICOLON>
#							exit\")
							
#######################################################################
#set(OOCD_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/boards/devusb_stm32f103c8t6/oocd_stlinkv2.cfg")
set(OOCD_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/boards/devusb_stm32f103c8t6/oocd_jlink.cfg")
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md

# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${OOCD_CONFIG} -c \"do flash\")

#######################################################################
# Определение дополнительной цели для выполнения операции отладки
add_custom_target("debug" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором

# Определение команд для цели flash
add_custom_command(	TARGET "debug"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${OOCD_CONFIG} -c \"do debug\")
							
