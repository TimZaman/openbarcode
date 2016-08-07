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
---------------------------------------------------------------------------------
*/

#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include <unistd.h>
#include <sys/types.h>

#include <opencv2/opencv.hpp>

#include "libopenbarcode/openbarcode_version.h"
#include "libopenbarcode/options.h"  

#include "libopenbarcode/detector.h" 
#include "libopenbarcode/detector_barcode.h" 

#include "libopenbarcode/decoder.h" 
#include "libopenbarcode/decoder_code39.h" 

using namespace std;

int main(int argc, char* argv[] ) {
	std::cout << "main() : this program provides a libopenbarcode example." << std::endl;
	std::cout << "OPENBARCODE_VERSION   = " << OPENBARCODE_VERSION << std::endl;
	std::cout << "OPENBARCODE_BUILDDATE = " << OPENBARCODE_BUILDDATE << std::endl;

	// Load a sample image
	cv::Mat im = cv::imread("../../sample_images/c39.png");

	// Create the options
	openbarcode::Options opts;
	//opts.set

	opts.setValue("int", 1337);
	opts.setValue("string", "test");

	//cout << opts.getValue("int") << " ; " << opts.getValue("string") << endl;
	cout << opts.getValue<int>("int") << endl;
	int i = opts.getValue<int>("int");

	cout << opts.getValue<int>("default-test", 1234) << endl;

	// Create the decoder(s)
	std::vector< openbarcode::Decoder * > decoders;
	decoders.push_back(new openbarcode::DecoderCode39(&opts));

	// Create the detector by passing in the options and decoder
	openbarcode::DetectorBarcode dt_bc(&opts, decoders);

	dt_bc.setImage(im);

	dt_bc.Detect();

	dt_bc.Decode();

	for (int i = 0; i < decoders.size(); i++) {
		delete decoders[i];
	}


	std::cout << "END main()" << std::endl;
}
