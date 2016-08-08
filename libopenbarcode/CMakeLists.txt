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
# -   Module      :   CmakeLists.txt of the main library
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


project( libopenbarcode )
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")    

MESSAGE( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} " ON SYSTEM " ${CMAKE_SYSTEM_NAME} )

if(APPLE) #For MacPorts
    SET(CMAKE_PREFIX_PATH "/opt/local/lib/cmake" ${CMAKE_PREFIX_PATH})
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}) # make local *.cmake available

LINK_DIRECTORIES( ${LINK_DIRECTORIES}  /usr/local/lib /usr/lib /opt/local/lib)
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES}  /usr/local/include /usr/include /opt/local/include)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(GITVERSIONHDR ${CMAKE_CURRENT_BINARY_DIR}/gitversion.h)

    
#OpenCV
find_package(OpenCV 3 REQUIRED)
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_imgcodecs  ) #opencv3


if (APPLE)
    SET(CMAKE_PREFIX_PATH "/opt/local/lib/cmake" ${CMAKE_PREFIX_PATH})
    if(POLICY CMP0042) #No idea what does does but something with rpath or MACOSX_RPATH
       cmake_policy(SET CMP0042 OLD)
    endif()

    if(POLICY CMP0043) #No idea what does does but supresses warnings bonanza
       cmake_policy(SET CMP0043 OLD)
    endif()
endif()

if (NOT APPLE)
    include_directories(/usr/lib/x86_64-linux-gnu)
endif()

add_definitions(-DMYSHAREDLIB_LIBRARY)

# Add an library compiled from .cpp
add_library( openbarcode SHARED 
    detector.cpp detector_barcode.cpp detector_dmtx.cpp
    decoder.cpp decoder_code39.cpp  decoder_dmtx.cpp
    toolkit/sort.h toolkit/utils_opencv.cpp toolkit/utils_general.cpp
    )

# Link the target to the libraries
target_link_libraries( openbarcode ${OpenCV_LIBS} dmtx stdc++ c++ )


# GIT STUFF
# include the output directory, where the gitversion.h file is generated
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# a custom target that is always built
add_custom_target(githeader ALL DEPENDS ${GITVERSIONHDR})
# creates githeader.h using cmake script
add_custom_command(OUTPUT ${GITVERSIONHDR} COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/getgit.cmake)
# gitversion.h is a generated file
set_source_files_properties(${GITVERSIONHDR} PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
# explicitly say that the executable depends on the githeader
add_dependencies(openbarcode githeader)

if(APPLE)
    INSTALL(FILES _build/libopenbarcode.dylib  DESTINATION /usr/local/lib/ )
else() #Linux
    INSTALL(FILES _build/libopenbarcode.so  DESTINATION /usr/local/lib/ )
endif()

INSTALL(FILES openbarcode.h openbarcode_version.h DESTINATION /usr/local/include/libopenbarcode/ ) 






