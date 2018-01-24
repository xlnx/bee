cmake_minimum_required(VERSION 3.6)
project(bee)
set(CMAKE_CXX_STANDARD 14)

include_directories(
	"./dependency"
	"./include"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses")

IF (WIN32)
	find_package(OpenGL REQUIRED)
	link_libraries(glew32.lib glfw3.dll ${OPENGL_LIBRARIES} )
	set(CMAKE_CXX_FLAGS_DEBUG "-Wall ${CMAKE_CXX_FLAGS} -DDEBUG -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -mwindows")
ENDIF ()

set(SOURCE_FILES
	src/util.cpp
	src/runtime.cpp
	test/testRuntime.cpp
)

add_executable(bee ${SOURCE_FILES})