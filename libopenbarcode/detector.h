/**
--------------------------------------------------------------------------------
-   Module      :   detector.h
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

#ifndef OPENBARCODE_DETECTOR_H
#define OPENBARCODE_DETECTOR_H

#include <string>
#include <vector>

#include "libopenbarcode/options.h"  
#include "libopenbarcode/decoder.h"

namespace openbarcode {

class Detector {

 public:
    Detector(Options *, Decoder *);
    Detector(Options *, std::vector< Decoder * >);
    ~Detector();

    virtual int Detect();

    //template<typename T>
    //int setOption(int, const T);

 private:
    /*
     * codetype_ is the type of code it should look for with this detector
     */
    int codetype_;
    std::vector< Decoder *> decoders_;
    Options * opts_;
        
};

}


#endif








