cmake_minimum_required(VERSION 2.8.11)

project( openbarcode-distribution )

# Force out-of-source build
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)


set(dir ${CMAKE_CURRENT_SOURCE_DIR}./_build)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

option(BUILD_LIB  "Builds the openbarcode library" ON)
option(BUILD_CLI  "Builds the openbarcode cli" ON)
option(BUILD_TEST "Builds the openbarcode tests" ON)

if(BUILD_LIB)
	add_subdirectory( libopenbarcode )
endif()

if(BUILD_CLI)
	add_subdirectory( openbarcode-cli )
endif()

if(BUILD_TEST)
	add_subdirectory( tests )
endif()
