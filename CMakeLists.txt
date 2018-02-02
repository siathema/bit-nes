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
endif()

set(SOURCES
  src/b_main.cc
  src/b_6502.cc
  src/b_ppu.cc
  src/b_utils.cc
  src/b_log.cc )

add_executable(bit-nes ${SOURCES})
