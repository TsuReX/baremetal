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
#set(MAIN_ASM_SOURCES    "${CMAKE_CURRENT_SOURCE_DIR}/boards/stellaris_lm4f120h5qr/source/startup.s" )

set(MAIN_SOURCES	"${MAIN_SOURCES}"
					## TODO Реализовать механизмы инициализации в файле и подключить его к сборке
					##"${CMAKE_CURRENT_SOURCE_DIR}/boards/stellaris_lm4f120h5qr/source/system_tm4c1233h6pm.c"
					"${CMAKE_CURRENT_SOURCE_DIR}/boards/stellaris_lm4f120h5qr/source/startup_tm4c1233h6pm.c"
					"${CMAKE_CURRENT_SOURCE_DIR}/source/stellaris_main.c"
					)
					
set(DEVICE_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/base/Device/TI/TM4C/Include")

#set(DRIVER_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/base/Driver/ST/stm32f3xx/include")

#file(GLOB DRIVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/base/Driver/ST/stm32f3xx/source/*_ll_*.c")

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS	"${CMAKE_C_FLAGS} -mcpu=cortex-m4")
	
set(CMAKE_ASM_FLAGS	"${CMAKE_ASM_FLAGS} -mcpu=cortex-m4")
	
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}	-T ${CMAKE_CURRENT_SOURCE_DIR}/boards/stellaris_lm4f120h5qr/flash_tm4c1233h6pm.ld \
														-mcpu=cortex-m4 -specs=nano.specs \
														-Wl,--gc-sections")
								
add_definitions("-DTM4C1233H6PM")

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
					ARGS	-f ${OOCD_CONFIG} -c \"init$<SEMICOLON>
							reset halt$<SEMICOLON>
							flash write_image erase ${CMAKE_BINARY_DIR}/${PROJ_NAME}${CMAKE_EXECUTABLE_SUFFIX}$<SEMICOLON>
							reset$<SEMICOLON>
							exit\")
