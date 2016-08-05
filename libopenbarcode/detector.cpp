/**
--------------------------------------------------------------------------------
-   Module      :   detector.cpp
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 18-FEB-2016
--------------------------------------------------------------------------------
*/

#include <limits>
#include <map>
#include <clocale>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

#include "libopenbarcode/toolkit/sort.h"

#include "libopenbarcode/detector.h"

namespace openbarcode {


Detector::Detector(Options *opts, Decoder * dc) : opts_(opts){
    decoders_.push_back(dc);
    std::cout << "Detector::Detector()" << std::endl;
}

Detector::Detector(Options *opts, std::vector< Decoder * > decoders) : opts_(opts), decoders_(decoders){
    std::cout << "Detector::Detector()" << std::endl;
}

Detector::~Detector(){
    std::cout << "Detector::~Detector()" << std::endl;

}

//template<typename T>
//int Detector::setOption(int opt_id, const T value){
//
//    return 1;
//}

int Detector::Detect(){
    std::cout << "Detector::Detect()" << std::endl;
    std::cerr << " please override this function" << std::endl;
    return RET_NONE_FOUND;
}



} //END NAMESPACE openbarcode








