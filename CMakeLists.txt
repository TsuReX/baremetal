## Указание минимальной необходимой версии cmake для сборки проекта

cmake_minimum_required(VERSION 3.5)

string(TOLOWER ${BOARD_TYPE} BOARD_TYPE_LOWER_CASE)

## Переменная служит индикатором полной настройки
set(BOARD_TYPE_STATUS "NOT_SET")

#######################################################################
## Загрузка вспомогательных скриптов для настройки параметров сборки
if(${BOARD_TYPE_LOWER_CASE} STREQUAL "bluepill_stm32f103c8t6")

	set(PROJ_NAME bluepill_stm32f103c8t6)

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	set(CMAKE_C_COMPILER_PREFIX "arm-none-eabi-")
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")

	SET(SUB_PROJECTS	"${SUB_PROJECTS}"
						"${CMAKE_CURRENT_SOURCE_DIR}/ringbuf")

	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ringbuf/include")
	set(LIBS "ringbuf")

#######################################################################
elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "imx8m")

	set(PROJ_NAME imx8m)

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	set(CMAKE_C_COMPILER_PREFIX "aarch64-none-elf-")
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")

#######################################################################
elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "x86")

	set(PROJ_NAME x86)
#	project(${PROJ_NAME} C NASM)

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

#	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "nasm")

#######################################################################
elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "hostusb_mdr32f9")

	set(PROJ_NAME hostusb_mdr32f9)

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	set(CMAKE_C_COMPILER_PREFIX "arm-none-eabi-")
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")

#######################################################################
elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "dbs")

	set(PROJ_NAME "${BOARD_TYPE_LOWER_CASE}")

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	set(CMAKE_C_COMPILER_PREFIX "arm-none-eabi-")
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")

#	SET(SUB_PROJECTS	"${SUB_PROJECTS}"
#						"${CMAKE_CURRENT_SOURCE_DIR}/ringbuf"
#					)

#	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ringbuf/include")
#	set(LIBS "ringbuf")

######################################################################
elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "baikalbmc_stm32f103ret6")

	set(PROJ_NAME baikalbmc_stm32f103ret6)

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	set(CMAKE_C_COMPILER_PREFIX "arm-none-eabi-")
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")

	SET(SUB_PROJECTS	"${SUB_PROJECTS}"
						"${CMAKE_CURRENT_SOURCE_DIR}/ringbuf")

	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ringbuf/include")
	set(LIBS "ringbuf")

######################################################################
elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "baikalbmc_stm32l412kbu6")

	set(PROJ_NAME baikalbmc_stm32l412kbu6)

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	set(CMAKE_C_COMPILER_PREFIX "arm-none-eabi-")
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")

	SET(SUB_PROJECTS	"${SUB_PROJECTS}"
						"${CMAKE_CURRENT_SOURCE_DIR}/ringbuf")

	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ringbuf/include")
	set(LIBS "ringbuf")
######################################################################
elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "atbrk3568jsmcbmc_stm32l071kbu6")

	set(PROJ_NAME atbrk3568jsmcbmc_stm32l071kbu6)

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	set(CMAKE_C_COMPILER_PREFIX "arm-none-eabi-")
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")

#	SET(SUB_PROJECTS	"${SUB_PROJECTS}"
#						"${CMAKE_CURRENT_SOURCE_DIR}/ringbuf")

#	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ringbuf/include")
#	set(LIBS "ringbuf")
#######################################################################
elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "pulsox_stm32l4r9aii6u")

	set(PROJ_NAME pulsox_stm32l4r9aii6u)

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	set(CMAKE_C_COMPILER_PREFIX "arm-none-eabi-")
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")

	SET(SUB_PROJECTS	"${SUB_PROJECTS}"
						"${CMAKE_CURRENT_SOURCE_DIR}/ringbuf")

	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ringbuf/include")
	set(LIBS "ringbuf")

######################################################################
elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "som-carrier")

	set(PROJ_NAME som-carrier)

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	set(CMAKE_C_COMPILER_PREFIX "avr-")
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")
	set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER_PREFIX}gcc")

######################################################################
endif()

if (NOT EXISTS ${BOARD_SCRIPT_CMAKE})
	message(FATAL_ERROR " Auxilary cmake script ${BOARD_SCRIPT_CMAKE} couldn't be found.\n")
endif()

## Предотвращение проверки работоспособности компилятора
## (необходимо для избежания проблемы при кросс-сборке)
set(CMAKE_C_COMPILER_WORKS 1)

## Указание используемых в проекте языков
project(${PROJ_NAME} C ASM)

message("-- MAIN_SOURCES ${MAIN_SOURCES}")
set(CMAKE_EXECUTABLE_SUFFIX .elf )

if (DEFINED SUB_PROJECTS)
	add_subdirectory(${SUB_PROJECTS})
endif ()

include(${BOARD_SCRIPT_CMAKE})

include_directories(${INCLUDE})

#######################################################################
# Переменная описывает имя и положение фала с конфигурацией OOCD для работы с конкретной платформой-процессором
# Смотреть FLASHER_TYPE в README.md

if (NOT DEFINED PROGRAMMER)
	set(PROG_SCRIPT	"oocd_stlinkv2.cfg")
else()
	if (${PROGRAMMER} STREQUAL "ST2")
		set(PROG_SCRIPT	"oocd_stlinkv2.cfg")
	elseif(${PROGRAMMER} STREQUAL "JLINK")
		set(PROG_SCRIPT	"oocd_jlink.cfg")
	else()
		return()
	endif ()
endif ()

#######################################################################
# Определение дополнительной цели для выполнения операции прошивки
add_custom_target("flash" DEPENDS ${PROJ_NAME})
# Определение команд для цели flash
add_custom_command(	TARGET "flash"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BOARD_SRC_PATH}/${PROG_SCRIPT} -c \"do flash ${PROJ_NAME}\")

# Определение дополнительной цели для выполнения операции отладки
add_custom_target("debug" DEPENDS ${PROJ_NAME})
# Определение команд для цели flash
add_custom_command(	TARGET "debug"
					POST_BUILD
					COMMAND openocd
					ARGS	-f ${BOARD_SRC_PATH}/${PROG_SCRIPT} -c \"do debug ${PROJ_NAME}\")
		
					
#######################################################################
## Проверка установки параметров: 
## BOARD_TYPE_STATUS, CMAKE_C_COMPILER

# Если указан тип платформы, а вспомогательный скрипт не найден,
# то выдать сообщение и завершить работу
if (${BOARD_TYPE_STATUS} STREQUAL "NOT_SET")
	message(FATAL_ERROR " Unknown BOARD_TYPE was specified.\n"
	" Specifiy correct BOARD_TYPE or specify parameters manualy.")
endif()

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")

#######################################################################
## Сборка и компоновка

add_executable(${PROJ_NAME} ${MAIN_SOURCES})

target_link_libraries(${PROJ_NAME} ${LINKER_LIBS} ${LINKER_FLAGS} ${LIBS})

add_custom_command(
        TARGET ${PROJ_NAME} POST_BUILD
        COMMAND "${CMAKE_C_COMPILER_PREFIX}objcopy" -O binary
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.elf
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.bin)

add_custom_command(
        TARGET ${PROJ_NAME} POST_BUILD
        COMMAND "${CMAKE_C_COMPILER_PREFIX}objdump" -SD
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.elf >
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.elf.objdump)

#if(${BOARD_TYPE_LOWER_CASE} STREQUAL "x86")
#
#    add_custom_command(
#	TARGET ${PROJ_NAME} PRE_BUILD
#	COMMAND "${CMAKE_C_COMPILER_PREFIX}objcopy" -I binary -O elf32-i386
#	    ${MICROCODE_UPDATE}
#	    ${CMAKE_CURRENT_BINARY_DIR}/microcode_update.out
#    )
#endif()