# --------------------------------------------------------------------------------------
#                                 OpenBarcode 
#                         1D and 2D C++ Barcode Library
#                                                                                                  
#                ____ ____ ____ ____ ____ ____ ____ ____ ____ ____ ____ 
#               ||o |||p |||e |||n |||b |||a |||r |||c |||o |||d |||e ||
#               ||__|||__|||__|||__|||__|||__|||__|||__|||__|||__|||__||
#               |/__\|/__\|/__\|/__\|/__\|/__\|/__\|/__\|/__\|/__\|/__\|
#
#
# --------------------------------------------------------------------------------------
# -   Module      :   CmakeLists.txt of the example cli
# -   Description :   CMakeList of OpenBarcode
# -   Author      :   Tim Zaman, 18-FEB-2016
# --------------------------------------------------------------------------------
# 
# Copyright (c) 2016 Tim Zaman
# 
# Permission to use, copy, modify, distribute, and sell this software
# for any purpose is hereby granted without fee, provided
# that (i) the above copyright notices and this permission notice appear in
# all copies of the software and related documentation.
# 
# THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
# EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
# WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# 
# -------------------------------------------------------------------------------------- 

project( openbarcode )
cmake_minimum_required(VERSION 2.8.11)

if (APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
	if(POLICY CMP0043) #No idea what does does but supresses warnings bonanza
		cmake_policy(SET CMP0043 OLD)
	endif()
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

MESSAGE( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} " ON SYSTEM " ${CMAKE_SYSTEM_NAME} )

if (APPLE)
	INCLUDE_DIRECTORIES(/opt/local/include)
	LINK_DIRECTORIES(/opt/local/lib)
else()
	INCLUDE_DIRECTORIES(/usr/include/)
	INCLUDE_DIRECTORIES(/usr/local/include/)
	LINK_DIRECTORIES(/usr/lib/x86_64-linux-gnu/)
	LINK_DIRECTORIES(/usr/lib)
endif()

if (1) # USE LOCAL COMPILED VERSION
	#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libopenbarcode)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..)
	LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../lib)
else () # USE INSTALLED LIB AND HEADERS
	INCLUDE_DIRECTORIES(/usr/local/include/openbarcode/ )
	LINK_DIRECTORIES(/usr/local/lib)
endif()

#OpenCV
find_package(OpenCV 3 REQUIRED)
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_imgcodecs  ) #opencv3

# Add an executable compiled from .cpp
add_executable( openbarcode-bin main.cpp ) #openbarcode-bin name is a workaround, see below
SET_TARGET_PROPERTIES(openbarcode-bin PROPERTIES OUTPUT_NAME openbarcode) #workaround to have executable and library with same name (openbarcode)

# Link the target to the libraries
target_link_libraries( openbarcode-bin openbarcode stdc++ ${OpenCV_LIBS})
