cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "stellaris_lm4f120h5qr")

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
set(BRD_PATH 			"${CMAKE_CURRENT_SOURCE_DIR}/boards/stellaris_lm4f120h5qr")

#set(MAIN_ASM_SOURCES    "${CMAKE_CURRENT_SOURCE_DIR}/boards/stellaris_lm4f120h5qr/source/startup.s" )

set(MAIN_SOURCES		"${MAIN_SOURCES}"
						"${BRD_PATH}/source/startup_tm4c1233h6pm.c"
						"${BRD_PATH}/source/init.c"
						"${BRD_PATH}/source/platform.c"
						"${BRD_PATH}/source/console.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/time.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/debug.c"
						"${CMAKE_CURRENT_SOURCE_DIR}/source/stellaris_main.c"
				)
					
set(MAIN_INCLUDE		"${BRD_PATH}/include"
						"${CMAKE_CURRENT_SOURCE_DIR}/include/"
				)

set(CORE_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/core/include")

set(DEVICE_INCLUDE 		"${CMAKE_CURRENT_SOURCE_DIR}/base/device/ti/tm4c/include")

set(DRIVER_INCLUDE		"${CMAKE_CURRENT_SOURCE_DIR}/base/driver/ti/tm4c/include")

file(GLOB DRIVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/base/driver/ti/tm4c/source/*.c")

file(GLOB DRIVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/base/driver/ti/tm4c/driverlib/*.c")

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS	"${CMAKE_C_FLAGS} -mcpu=cortex-m4 -g -ggdb")
	
set(CMAKE_ASM_FLAGS	"${CMAKE_ASM_FLAGS} -mcpu=cortex-m4 -g -ggdb")
	
set(LINKER_FLAGS		"${LINKER_FLAGS}"
						"-T ${BRD_PATH}/flash_tm4c1233h6pm.ld"
						"-mcpu=cortex-m4 -specs=nano.specs"
#						"-Wl,--gc-sections"
#						"-nostdlib"
				)

set(LINKER_LIBS 		"-lc"
						"-lm"
						"-lnosys"
				)
					
add_definitions("-DTM4C1233H6PM")
add_definitions("-DTARGET_IS_TM4C123_RA1")
add_definitions("-DPART_TM4C1233H6PM")
add_definitions("-Dgcc")

#######################################################################
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md
set(OOCD_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/boards/stellaris_lm4f120h5qr/oocd_ti-icdi.cfg")

# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BRD_PATH}/oocd_ti-icdi.cfg -c \"do flash\")
#######################################################################
# Определение дополнительной цели для выполнения операции отладки
add_custom_target("debug" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором

# Определение команд для цели debug
add_custom_command(	TARGET "debug"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BRD_PATH}/oocd_ti-icdi.cfg -c \"do debug\")
