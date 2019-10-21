cmake_minimum_required(VERSION 2.8)

message("-- MSG: Add dir ${CMAKE_CURRENT_SOURCE_DIR}")

set(PROJ_NAME debug-dev)

project(${PROJ_NAME} C ASM)

set(CMAKE_C_FLAGS "-mcpu=arm1176jzf-s -fpic -ffreestanding -ggdb -g")

set(CMAKE_ASM_FLAGS "-mcpu=arm1176jzf-s -fpic -ffreestanding -ggdb -g")

set( CMAKE_EXECUTABLE_SUFFIX .elf )
set( CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/map.ld -nostdlib")

add_subdirectory(device)

set(C_SRC	)

set(ASM_SRC	start.S
		)

include_directories("include")

add_executable(${PROJ_NAME} ${ASM_SRC} ${C_SRC})

target_link_libraries(${PROJ_NAME} device)
