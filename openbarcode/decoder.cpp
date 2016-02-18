/**
--------------------------------------------------------------------------------
-   Module      :   decoder.h
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 18-FEB-2016
--------------------------------------------------------------------------------
*/

#include <limits>
#include <map>
#include <clocale>
#include <assert.h>
#include <iostream>

#include "openbarcode/decoder.h"

namespace openbarcode {


Decoder::Decoder(){
	std::cout << "Decoder::Decoder()" << std::endl;
}

Decoder::~Decoder(){
	std::cout << "Decoder::~Decoder()" << std::endl;

}

template<typename T>
int Decoder::setOption(int opt_id, const T value){

}



} //END NAMESPACE openbarcode