/**
--------------------------------------------------------------------------------
-   Module      :   main.cpp
-   Description :   Example of cli usage of 1D and 2D C++ Barcode Library
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

#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include <unistd.h>
#include <sys/types.h>
 
#include "openbarcode/detector.h" 
#include "openbarcode/decoder.h" 


int main( int argc, char* argv[] ){
	std::cout << "main() : this program provides a libopenbarcode test." << std::endl;

	openbarcode::Decoder dc;
	openbarcode::Detector dt;


	std::cout << "END main()" << std::endl;
}









