/**
--------------------------------------------------------------------------------
-   Module      :   detector.h
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 18-FEB-2016
--------------------------------------------------------------------------------
*/

#include <limits>
#include <map>
#include <clocale>
#include <assert.h>
#include <iostream>

#include "openbarcode/detector.h"

namespace openbarcode {


Detector::Detector(){
	std::cout << "Detector::Detector()" << std::endl;
}

Detector::~Detector(){
	std::cout << "Detector::~Detector()" << std::endl;

}

template<typename T>
int Detector::setOption(int opt_id, const T value){

}



} //END NAMESPACE openbarcode