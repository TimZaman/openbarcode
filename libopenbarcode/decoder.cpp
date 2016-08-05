/**
--------------------------------------------------------------------------------
-   Module      :   decoder.cpp
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 18-FEB-2016
--------------------------------------------------------------------------------
*/

#include <iostream>

#include "libopenbarcode/decoder.h"

namespace openbarcode {

Decoder::Decoder(Options * opts) : opts_(opts) {
    std::cout << "Decoder::Decoder()" << std::endl;
}

Decoder::~Decoder(){
    std::cout << "Decoder::~Decoder()" << std::endl;

}

int Decoder::Decode(cv::Mat image, openbarcode::code * current_code){
    std::cout << "Decoder::Decode()" << std::endl;
}

} //END NAMESPACE openbarcode
