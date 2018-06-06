cmake_minimum_required(VERSION 2.6)
project(bit-nes)

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(LINUX)
	set(CMAKE_C_COMPILER /usr/bin/clang)
	set(CMAKE_CXX_COMPILER /usr/bin/clang++)
	set(CMAKE_CXX_FLAGS "-Wall -g -DDEBUG_PRINT -std=c++11 -fcolor-diagnostics")
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
else() #MSVC
	set(CMAKE_CXX_FLAGS "-DWIN32=1 -DWIN64=1 /EHsc /Zi /MP ") #/F NUM to change default stack size.
	set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG_PRINT=1 /MTd /Od")
endif()

set(SOURCES
	src/b_main.cc
	src/b_6502.cc
	src/b_ppu.cc
	src/b_utils.cc
	src/b_log.cc
	src/bit_nes.cc )

add_executable(bit-nes ${SOURCES})
