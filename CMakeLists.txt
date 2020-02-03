## Указание минимальной необходимой версии cmake для сборки проекта

cmake_minimum_required(VERSION 3.5)

## Установки имени проекта

set(PROJ_NAME debug-dev)

## Предотвращение проверки работоспособности компилятора
## (необходимо для избежания проблемы при кросс-сборке)

set(CMAKE_C_COMPILER_WORKS 1)

## Указание используемых в проекте языков

project(${PROJ_NAME} C ASM)

#######################################################################
## Проверка установки параметров: 
## BOARD_TYPE_STATUS, CMAKE_C_COMPILER, SOC_TYPE

# Если указан тип платформы, а вспомогательный скрипт не найден,
# то выдать сообщение и завершить работу
if (NOT DEFINED BOARD_TYPE)
	message(FATAL_ERROR " Unknown BOARD_TYPE was specified.\n"
	" Specifiy correct BOARD_TYPE or specify parameters manualy.")
endif()

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")

if (NOT DEFINED SOC_TYPE)
	message(FATAL_ERROR " SOC_TYPE must be defined.\n"
	" Ex. -DSOC_TYPE=STM32F303K8T6")
endif ()



#######################################################################
## Подключение файло исходных кодов и заголовков

set(MAIN_INCLUDE	"include")

set(MAIN_SOURCES	"source/main.c"
					"source/config.c"
					"source/console.c"
					"source/scheduler.c"
)

set(CORE_INCLUDE "base/Core/Include")

if(${BOARD_TYPE} STREQUAL "NUCLEO_F303K8T6")

	set(SUBSCRIPT "boards/nuclef303k8t6.cmake")

	if (NOT EXISTS ${SUBSCRIPT})
		message(FATAL_ERROR " Auxilary cmake script ${SUBSCRIPT} couldn't be found.\n")
	endif()

	include(${SUBSCRIPT})

elseif(${BOARD_TYPE} STREQUAL "CHINA_BIG_F407VET6")

	##	TODO Сделать аналогично NUCLEO_F303K8T6

endif()

## TODO Перенести в boards/nuclef303k8t6.cmake
if (NOT ${SOC_TYPE} STREQUAL "STM32F303K8T6")
	
	set(MAIN_ASM_SOURCES	"src/startup_stm32f303x8.s" )
	
	set(MAIN_SOURCES	"${MAIN_SOURCES}"
						"source/stm32f3xx_it.c"
						"source/system_stm32f3xx.c"
					
	set(DEVICE_INCLUDE "base/Device/ST/STM32F3xx/Include")
	
	set(DRIVER_INCLUDE "base/Driver/ST/stm32f3xx/include")
	
##if (DEFINED USE_FULL_LL_DRIVER AND USE_FULL_LL_DRIVER STREQUAL "y")
##	add_definitions("-DUSE_FULL_LL_DRIVER")
##	message(STATUS "Full driver is used")
##endif ()
	AUX_SOURCE_DIRECTORY(DRIVER_SOURCES "base/Driver/ST/stm32f3xx/sources")

endif ()


include_directories(${MAIN_INCLUDE})
include_directories(${CORE_INCLUDE})
include_directories(${DEVICE_INCLUDE})
include_directories(${DRIVER_INCLUDE})

#######################################################################
## Настройка параметров сбоки и компоновки

if (NOT ${SOC_TYPE} STREQUAL "STM32F303K8T6")

	set(CMAKE_C_FLAGS	"-mcpu=cortex-m4 -ggdb -g -Werror")
	
	set(CMAKE_ASM_FLAGS	"-mcpu=cortex-m4 -ggdb -g -Werror")
	
	set(CMAKE_EXECUTABLE_SUFFIX .elf )
	
	set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/flash.ld \
								-mcpu=cortex-m4 -specs=nano.specs \
								-Wl,--gc-sections")
								
	add_definitions("-DSTM32F303x8")
	
endif ()

#######################################################################
## Подключение дополнительных библиотек (git-модулей)

# TODO: Реализовать добавление подмодулей git и включение заголовочников через список,
# чтобы исключить здесь ручное добавление кода для каждого подмодуля 

add_subdirectory("ringbuf")

include_directories("ringbuf/include")

#######################################################################
## Сборка и компоновка

add_executable(${PROJ_NAME} ${MAIN_ASM_SOURCES} ${MAIN_SOURCES} ${DRIVER_SOURCES})

target_link_libraries(${PROJ_NAME} -lc -lm -lnosys ringbuf)
