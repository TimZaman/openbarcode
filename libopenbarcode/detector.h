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
---------------------------------------------------------------------------------
*/

#ifndef OPENBARCODE_DETECTOR_H
#define OPENBARCODE_DETECTOR_H

#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "libopenbarcode/options.h"  
#include "libopenbarcode/decoder.h"

namespace openbarcode {

class Detector {

 public:
    Detector(Options *, Decoder *);
    Detector(Options *, std::vector< Decoder * >);
    ~Detector();

    void setImage(cv::Mat);

    virtual int Detect();

    int Decode();

    int numCodesFound() { // @TODO(tzaman): put in src
        return codes_.size();
    }

    std::vector< std::string > getCodeStrings() { // @TODO(tzaman): put in src
        std::vector< std::string > only_strings(codes_.size());
        for (int i = 0; i < codes_.size(); i++) {
            only_strings[i] = this->codes_[i].data;
        }
        return only_strings;
    }

    cv::Mat drawAllCandidates();

 protected:
    std::vector< openbarcode::code > code_candidates_;
    std::vector< openbarcode::code > codes_;
    std::vector< openbarcode::code > dt_bc;
    std::vector< Decoder *> decoders_;
    Options * opts_;
    cv::Mat image_;
 private:
    DISALLOW_COPY_AND_ASSIGN(Detector);
    std::map<int, int> rejection_counter_;

};

}

#endif
