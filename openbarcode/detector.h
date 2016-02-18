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
 --------------------------------------------------------------------------------
*/

#ifndef DETECTOR_H
#define DETECTOR_H

#include <string>
#include <vector>

#include "openbarcode_version.h"
#include "openbarcode/decoder.h"

namespace openbarcode {

/*
 * Return values
 */
enum { RET_SUCCESS   = 0, 

     };

/*
 * All possible option keys
 */
enum { OPT_CODETYPE   = 0,
     };

/*
 * Code type values for the kind of code to detect
 */ 
enum { DET_BARCODE    = 1<<0, // 0x01 (any code with bars)
       DET_DATAMATRIX = 1<<1, // 0x02
       DET_QR         = 1<<2, // 0x04
     };


class Detector {

	public:
		Detector();
		~Detector();

		int Detect();

		template<typename T>
		int setOption(int opt_id, const T value);

	private:
		/*
		 * codetype_ is the type of code it should look for with this detector
		 */
		int codetype_;
		
		
};

}


#endif








