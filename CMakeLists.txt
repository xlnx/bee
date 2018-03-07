cmake_minimum_required(VERSION 3.5)
project(bee)
set(CMAKE_CXX_STANDARD 14)

option(BUILD_TESTS "Build tests." TRUE)

add_definitions(
	-DBEE_RUNTIME_INTRUSIVE
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses")
# if (BEE_RUNTIME_NO_CONSOLE)
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
# endif ()
if (WIN32)
	find_package(OpenGL REQUIRED)
	link_libraries(glew32.lib glfw3.dll ${OPENGL_LIBRARIES})
	link_libraries(bfd iberty imagehlp z)
	set(CMAKE_CXX_FLAGS_DEBUG "-Wall -pg -DDEBUG -g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mwindows")
endif ()

link_libraries(assimp)

file(GLOB BEE_DIR "${CMAKE_SOURCE_DIR}/src/*")
file(GLOB BEE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include")
foreach (f ${BEE_DIR})
	string(REGEX REPLACE ".+/(.+)\\..*" "" TEST_V ${f})
	if (NOT ${TEST_V} STREQUAL "")
		set(BEE_INCLUDE_DIRS
			${BEE_INCLUDE_DIRS}
			${f}
		)
		file(GLOB_RECURSE TEMP_V "${f}/*.cpp")
		set(BEE_SOURCES
			${BEE_SOURCES}
			${TEMP_V}
		)
	endif ()
endforeach ()
file(GLOB BEE_TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")

include_directories(${BEE_INCLUDE_DIRS})

if (BUILD_TESTS)
	enable_testing()
	foreach (TEST_SOURCE ${BEE_TEST_SOURCES})
		string(REGEX REPLACE ".+/(.+)\\..*" "\\1" TEST_FILENAME ${TEST_SOURCE})
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY tests)
		add_executable(${TEST_FILENAME}
			${BEE_SOURCES}
			${TEST_SOURCE}
		)
		add_test(${TEST_FILENAME} Debug ${TEST_FILENAME})
		# message(building test file ${TEST_FILENAME})
	endforeach ()
endif ()

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "")
# file(GLOB BEE_MAIN "${CMAKE_SOURCE_DIR}/src/*.cpp")
# add_executable(bee
# 	${BEE_SOURCES}
# 	${BEE_MAIN}
# )
