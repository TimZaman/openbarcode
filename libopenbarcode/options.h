/**
--------------------------------------------------------------------------------
-   Module      :   options.h
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
---------------------------------------------------------------------------------
*/

#ifndef OPENBARCODE_OPTIONS_H
#define OPENBARCODE_OPTIONS_H

#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include <boost/variant.hpp> // Note this is a header-only library (no linking yay!)

#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                    \
  void operator=(const TypeName&) = delete;                                   \
  void operator=(TypeName&&) = delete;                                        \
  TypeName(const TypeName&) = delete;                                         \
  TypeName(TypeName&&) = delete

namespace openbarcode {


/*
 * Rejection Reasons
 */
enum { 
      REJECT_DEC_DMTX_TOO_FEW_TIMING_POINTS,
      REJECT_DEC_DMTX_DECODING_FAILED
     };

struct code {
    int code_type;
    std::string data;
    std::string data_raw;
    cv::RotatedRect rotrect;
    std::map<int, int> rejection_counter;
};

/*
 * Return values
 */
enum {
      RET_SUCCESS    = 0, 
      RET_NONE_FOUND = 1,
      RET_EPIC_FAIL = 1337
     };

/*
 * All possible option keys
 */
enum {
      OPT_CODETYPE   = 0,
      OPT_SEARCH_MULTIPLE = 1,
      OPT_DPI = 2,
     };

/*
 * Detector code types
 */ 
enum {
      DET_BARCODE    = 1 << 0, // 0x01 (any code with bars)
      DET_DATAMATRIX = 1 << 1, // 0x02
      DET_QR         = 1 << 2, // 0x04
     };

class Options {

 public:
    Options(){
        std::cout << "Options::Options()" << std::endl;

    }

    ~Options(){
        std::cout << "Options::~Options()" << std::endl;
    }

    template<typename _T>
    _T getValue(std::string key) {
        return boost::get<_T>(this->optionmap_.at(key)); // .at() throws exception when it doesnt exist
    }

    template<typename _T>
    _T getValue(std::string key, _T default_value) {
        // Uses a default value when no option key exists in the mapping
        if (this->optionmap_.count(key)) {
            return this->getValue<_T>(key);
        } else {
            return default_value;
        }
    }

    template<typename _T>
    int setValue(std::string key, _T value) {
        this->optionmap_[key] = value;
        return RET_SUCCESS; // @TODO(tzaman): check for duplicates etc
    }

 private:
    std::map<std::string, boost::variant<int, double, std::string> > optionmap_; // @TODO(tzaman): tuple with description?

};

}

#endif
