/**
--------------------------------------------------------------------------------
-   Module      :   detector_barcode.cpp
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 5-AUG-2016
--------------------------------------------------------------------------------
*/

#include <iostream>
#include <string>
#include <vector>

#include "libopenbarcode/detector_barcode.h"

namespace openbarcode {


DetectorBarcode::DetectorBarcode(Options * opts, Decoder * dc) : Detector(opts, dc) {
    std::cout << "DetectorBarcode::DetectorBarcode()" << std::endl;
}

DetectorBarcode::DetectorBarcode(Options * opts, std::vector< Decoder * > decoders) : Detector(opts, decoders) {
    std::cout << "DetectorBarcode::DetectorBarcode()" << std::endl;
}

DetectorBarcode::~DetectorBarcode() {
    std::cout << "DetectorBarcode::~DetectorBarcode()" << std::endl;

}

int DetectorBarcode::Detect() {
    std::cout << "DetectorBarcode::Detect()" << std::endl;

    return RET_NONE_FOUND;
}


} //END NAMESPACE openbarcode
