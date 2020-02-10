## Указание минимальной необходимой версии cmake для сборки проекта

cmake_minimum_required(VERSION 3.5)

## Установки имени проекта

set(PROJ_NAME debug-dev)

## Предотвращение проверки работоспособности компилятора
## (необходимо для избежания проблемы при кросс-сборке)

set(CMAKE_C_COMPILER_WORKS 1)

## Указание используемых в проекте языков

project(${PROJ_NAME} C ASM)


string(TOLOWER ${BOARD_TYPE} BOARD_TYPE_LOWER_CASE)

## Настройка параметров сбоки и компоновки
set(C_SRC	"src/main.c"
			"src/config.c"
			"src/console.c"
			"src/i2c.c"
			"src/scheduler.c"
			"src/stm32f3xx_it.c"
			"src/system_stm32f3xx.c"
			)

set(CMAKE_C_FLAGS	"-ggdb -g -Werror")
	
set(CMAKE_ASM_FLAGS	"-ggdb -g -Werror")

set(CMAKE_EXECUTABLE_SUFFIX .elf )

set(CMAKE_EXE_LINKER_FLAGS "")

## Переменная служит индикатором полной настройки
set(BOARD_TYPE_STATUS "NOT_SET")

#######################################################################
## Загрузка вспомогательных скриптов для настройки параметров сборки
if(${BOARD_TYPE_LOWER_CASE} STREQUAL "nucleo_stm32f303k8t6")
	
	## Подключение файлов исходных кодов и заголовков
	set(MAIN_INCLUDE	"include")
	
	set(MAIN_SOURCES	"source/nucleo_main.c"
						"source/config.c"
						"source/console.c"
						"source/scheduler.c"
	)
	
	set(CORE_INCLUDE	"base/Core/Include")

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	if (NOT EXISTS ${BOARD_SCRIPT_CMAKE})
		message(FATAL_ERROR " Auxilary cmake script ${BOARD_SCRIPT_CMAKE} couldn't be found.\n")
	endif()

	include(${BOARD_SCRIPT_CMAKE})


elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "stellaris_lm4f120h5qr")

	## Подключение файлов исходных кодов и заголовков
	set(MAIN_INCLUDE	"include")
	
	set(MAIN_SOURCES	"source/stellaris_main.c")
	
	set(CORE_INCLUDE	"base/Core/Include")

	set(BOARD_SCRIPT_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE_LOWER_CASE}/building.cmake")

	if (NOT EXISTS ${BOARD_SCRIPT_CMAKE})
		message(FATAL_ERROR " Auxilary cmake script ${BOARD_SCRIPT_CMAKE} couldn't be found.\n")
	endif()

	include(${BOARD_SCRIPT_CMAKE})

elseif(${BOARD_TYPE_LOWER_CASE} STREQUAL "china_stm32f407vet6")

	##	TODO Сделать аналогично china_stm32f407vet6

endif()

include_directories(${MAIN_INCLUDE})
include_directories(${CORE_INCLUDE})
include_directories(${DEVICE_INCLUDE})
include_directories(${DRIVER_INCLUDE})
						
#######################################################################
## Подключение дополнительных библиотек (git-модулей)

# TODO: Реализовать добавление подмодулей git и включение заголовочников через список,
# чтобы исключить здесь ручное добавление кода для каждого подмодуля 

add_subdirectory("ringbuf")

include_directories("ringbuf/include")

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

add_executable(${PROJ_NAME} ${MAIN_ASM_SOURCES} ${MAIN_SOURCES} ${DRIVER_SOURCES})

target_link_libraries(${PROJ_NAME} -lc -lm -lnosys ringbuf)
