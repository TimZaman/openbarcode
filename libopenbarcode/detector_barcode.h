/**
--------------------------------------------------------------------------------
-   Module      :   detector_barcode.h
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
 --------------------------------------------------------------------------------
*/

#ifndef OPENBARCODE_DETECTOR_BARCODE_H
#define OPENBARCODE_DETECTOR_BARCODE_H

#include <string>
#include <vector>
#include <map>

#include "libopenbarcode/options.h"
#include "libopenbarcode/detector.h"
#include "libopenbarcode/openbarcode_version.h"


namespace openbarcode {

class DetectorBarcode : public Detector {

 public:
    DetectorBarcode(Options *, Decoder *);
    DetectorBarcode(Options *, std::vector< Decoder * >);
    ~DetectorBarcode();

    int Detect();
};

}


#endif
