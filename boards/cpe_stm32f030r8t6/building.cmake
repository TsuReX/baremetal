cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "cpe_stm32f030r8t6")

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
set(BRD_PATH 			"${CMAKE_CURRENT_SOURCE_DIR}/boards/cpe_stm32f030r8t6")

set(MAIN_INCLUDE		"${BRD_PATH}/include"
						"${CMAKE_CURRENT_SOURCE_DIR}/include"
				)

set(BOARD_SRC_PATH		"${BRD_PATH}/source/")

set(MAIN_SOURCES		"${MAIN_SOURCES}"
						"${BRD_PATH}/source/startup_stm32f030r8t6.c"
						"${BRD_PATH}/source/system_stm32f0xx.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/cpe_main.c"
						"${BOARD_SRC_PATH}/init.c"
						"${BOARD_SRC_PATH}/console.c"
						"${BOARD_SRC_PATH}/scheduler.c"
				)

set(CORE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include")

set(DEVICE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/device/st/stm32f0xx/include")

set(DRIVER_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f0xx/include")

set(DRV_SOURCES_PATH 	"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/st/stm32f0xx/source")

set(DRIVER_SOURCES		"${DRV_SOURCES}"
						"${DRV_SOURCES_PATH}/stm32f0xx_ll_rcc.c"
						"${DRV_SOURCES_PATH}/stm32f0xx_ll_usart.c"
						"${DRV_SOURCES_PATH}/stm32f0xx_ll_utils.c"
				)

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS	"${CMAKE_C_FLAGS} -mcpu=cortex-m0 -Wall")
	
set(CMAKE_ASM_FLAGS	"${CMAKE_ASM_FLAGS} -mcpu=cortex-m0")
	
set(LINKER_FLAGS	"${LINKER_FLAGS}"
					"-T ${BRD_PATH}/flash.ld"
					"-mcpu=cortex-m0 -specs=nano.specs"
				)

set(LINKER_LIBS 	"-lc"
					"-lm"
					"-lnosys"
				)
				
add_definitions("-DSTM32F030x8")
add_definitions("-DUSE_FULL_LL_DRIVER")

######################################################################
# Определение дополнительной цели для выполнения операции прошивки
#add_custom_target("flash" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md
#set(OOCD_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/boards/nucleo_stm32f303k8t6/oocd_stlinkv2-1.cfg")

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
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME})

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

							
