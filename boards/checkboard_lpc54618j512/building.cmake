cmake_minimum_required(VERSION 3.5)

set(CURRENT_BOARD_TYPE "checkboard_lpc54618j512")

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
#set(MAIN_ASM_SOURCES    "${CMAKE_CURRENT_SOURCE_DIR}/boards/checkboard_lpc54618j512/source/startup_lpc54618.s" )

set(MAIN_SOURCES	"${MAIN_SOURCES}"
					## TODO Реализовать механизмы инициализации в файле и подключить его к сборке
					##"${CMAKE_CURRENT_SOURCE_DIR}/boards/checkboard_lpc54618j512/source/system_lpc54618.c"
					"${CMAKE_CURRENT_SOURCE_DIR}/boards/checkboard_lpc54618j512/source/startup_lpc54618.c"
					#"${CMAKE_CURRENT_SOURCE_DIR}/source/checkboad_main.c"
					)

set(MAIN_INCLUDE	${MAIN_INCLUDE}	"${CMAKE_CURRENT_SOURCE_DIR}/boards/checkboard_lpc54618j512/include")
				
set(DEVICE_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/base/Device/NXP/LPC546xx/Include")

set(DRIVER_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/base/Driver/NXP/LPC546xx/include")

file(GLOB DRIVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/base/Driver/NXP/LPC546xx/source/*.c")

#######################################################################
## Настройка параметров сбоки и компоновки

set(CMAKE_C_FLAGS	"${CMAKE_C_FLAGS} -mcpu=cortex-m4")
	
set(CMAKE_ASM_FLAGS	"${CMAKE_ASM_FLAGS} -mcpu=cortex-m4")
	
set(LINKER_FLAGS "${LINKER_FLAGS}	-T ${CMAKE_CURRENT_SOURCE_DIR}/boards/checkboard_lpc54618j512/flash_lpc54618j512.ld"
									"-mcpu=cortex-m4 -specs=nano.specs"
									"-Wl,--gc-sections")
								
#######################################################################
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME} "flash_programmer")

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md
set(OOCD_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/boards/checkboard_lpc54618j512/")

# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${OOCD_CONFIG_PATH}/oocd_jlink.cfg -f ${OOCD_CONFIG_PATH}/lpc546xx.cfg
							-c \"do flash\")
#					ARGS	-f ${OOCD_CONFIG} -c \"init$<SEMICOLON>
#							reset halt$<SEMICOLON>
#							flash write_image erase ${CMAKE_BINARY_DIR}/${PROJ_NAME}${CMAKE_EXECUTABLE_SUFFIX}$<SEMICOLON>
#							reset$<SEMICOLON>
#							exit\")

#######################################################################
# Определение дополнительной цели для выполнения операции отладки
add_custom_target("debug" DEPENDS ${PROJ_NAME})

# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором

# Определение команд для цели flash
add_custom_command(	TARGET "debug"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${OOCD_CONFIG_PATH}/oocd_jlink.cfg -f ${OOCD_CONFIG_PATH}/lpc546xx.cfg
							-c \"do debug\")
