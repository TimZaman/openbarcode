/**
--------------------------------------------------------------------------------
-   Module      :   options.h
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 18-FEB-2016
--------------------------------------------------------------------------------
-   Copyright (c) 2016 Tim Zaman
-   
-   Permission to use, copy, modify, distribute, and sell this software
-   for any purpose is hereby granted without fee, provided
-   that (i) the above copyright notices and this permission notice appear in
-   all copies of the software and related documentation.
-   
-   THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
-   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
-   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PAR-TICULAR PURPOSE.
---------------------------------------------------------------------------------
*/

#ifndef OPENBARCODE_OPTIONS_H
#define OPENBARCODE_OPTIONS_H

#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

namespace openbarcode {

struct code {
    int code_type;
    std::string data;
    std::string data_raw;
    cv::RotatedRect rotrect;
};

/*
 * Return values
 */
enum { RET_SUCCESS    = 0, 
       RET_NONE_FOUND = 1,

     };

/*
 * All possible option keys
 */
enum { OPT_CODETYPE   = 0,
       OPT_SEARCH_MULTIPLE = 1,
       OPT_DPI = 2,
     };

/*
 * Detector code types
 */ 
enum { DET_BARCODE    = 1<<0, // 0x01 (any code with bars)
       DET_DATAMATRIX = 1<<1, // 0x02
       DET_QR         = 1<<2, // 0x04
     };

class Options {

 public:
    Options(){
        std::cout << "Options::Options()" << std::endl;

    }

    ~Options(){
        std::cout << "Options::~Options()" << std::endl;
    }

 private:
    /*
     * codetype_ is the type of code it should look for with this detector
     */
    //int codetype_;
            
};

}

#endif
