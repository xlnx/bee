cmake_minimum_required(VERSION 3.5)
project(bee)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build tests" ON)

option(SMOOTH_COREDUMP "Use smooth coredump" ON)
option(OPENGL_CORE_PROFILE "Core profile" ON)
option(OPENGL_LOADER_GLAD "Use glad instead of glew" ON)

add_definitions(-DBEE_RUNTIME_INTRUSIVE)

if (${OPENGL_CORE_PROFILE})
	add_definitions(-DBEE_OPENGL_CORE_PROFILE)
endif ()
if (${SMOOTH_COREDUMP})
	add_definitions(-DBEE_SMOOTH_COREDUMP)
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses -Wno-noexcept-type -Wno-unused-variable -static-libgcc -static-libstdc++")

find_package(OpenGL REQUIRED)
link_libraries(glfw3 assimp ${OPENGL_LIBRARIES})
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -pg -DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if (WIN32)
	# link_libraries(glfw3.dll ${OPENGL_LIBRARIES})
	link_libraries(bfd iberty imagehlp z)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mwindows")
	if (NOT ${OPENGL_LOADER_GLAD})
		find_library(GLEW glew32)
	endif()
elseif (APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa -framework IOKit -framework CoreVideo")
	if (NOT ${OPENGL_LOADER_GLAD})
		find_library(GLEW glew)
	endif()
elseif (UNIX)
	link_libraries(X11 m dl Xxf86vm Xrandr Xi Xcursor Xinerama pthread)
endif ()

if (${OPENGL_LOADER_GLAD})
	add_definitions(-DBEE_OPENGL_LOADER_GLAD)
else()
	add_definitions(-DBEE_OPENGL_LOADER_GLEW)
	link_libraries(${GLEW})
	if (${OPENGL_CORE_PROFILE})
		message(FATAL_ERROR "GLEW does not support OpenGL core profile.")
	endif ()
endif ()
	
file(GLOB BEE_DIR "${CMAKE_SOURCE_DIR}/src/*")
file(GLOB BEE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include")
foreach (f ${BEE_DIR})
	string(REGEX REPLACE ".+/(.+)\\..*" "" TEST_V ${f})
	if (NOT ${TEST_V} STREQUAL "")
		set(BEE_INCLUDE_DIRS
			${BEE_INCLUDE_DIRS}
			${f}
		)
		file(GLOB_RECURSE TEMP_CPP "${f}/*.cpp")
		file(GLOB_RECURSE TEMP_C "${f}/*.c")
		set(BEE_SOURCES
			${BEE_SOURCES}
			${TEMP_CPP}
			${TEMP_C}
		)
	endif ()
endforeach ()
file(GLOB BEE_TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")

include_directories(${BEE_INCLUDE_DIRS})

if (${BUILD_TESTS})
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
