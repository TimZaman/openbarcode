/**
--------------------------------------------------------------------------------
-   Module      :   decoder.cpp
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 18-FEB-2016
--------------------------------------------------------------------------------
*/

//#include <map>
//#include <clocale>
//#include <assert.h>
#include <iostream>

#include "openbarcode/decoder.h"

namespace openbarcode {


Decoder::Decoder(Options * opts) : opts_(opts) {
	std::cout << "Decoder::Decoder()" << std::endl;
}

Decoder::~Decoder(){
	std::cout << "Decoder::~Decoder()" << std::endl;

}

//template<class T>
//int Decoder::setOption(int opt_id, T value){
//	return 1;
//}



} //END NAMESPACE openbarcode