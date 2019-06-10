cmake_minimum_required(VERSION 2.6)

project(bit-nes)

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(LINUX)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
	set(CMAKE_C_COMPILER /usr/bin/gcc)
	set(CMAKE_CXX_COMPILER /usr/bin/gcc)
	set(CMAKE_CXX_FLAGS "-Wall -g -DDEBUG_PRINT -std=c++11") #-fcolor-diagnostics")
	#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
else() #MSVC
	set(CMAKE_CXX_FLAGS "-DWIN32=1 -DWIN64=1 /EHsc /Zi /MP ") #/F NUM to change default stack size.
	set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG_PRINT=1 /MTd /Od")
endif()

set(SOURCES
	src/main.cc
	src/6502.cc
	src/ppu.cc
	src/utils.cc
	src/log.cc
	src/bit_nes.cc
	src/ops.cc)

  add_executable(bit-nes ${SOURCES})

  if(LINUX)
	target_link_libraries( bit-nes LINK_PUBLIC
                           ${OPENGL_LIBRARIES}
                           ${SDL2_LIBRARY}
                           GLEW
		   	   SDL2 )
  endif()
