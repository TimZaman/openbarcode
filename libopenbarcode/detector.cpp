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
#include "libopenbarcode/toolkit/utils_opencv.h"

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

void Detector::setImage(cv::Mat image) {
    std::cout << "Detector::setImage()" << std::endl;
    this->image_ = image;
    code_candidates_.clear();
}

int Detector::Detect() {
    std::cout << "Detector::Detect()" << std::endl;
    std::cerr << " please override this function" << std::endl;
    return RET_NONE_FOUND;
}

int Detector::Decode() {
    std::cout << "Detector::Decode()" << std::endl;
    
    // Loop over all candidates
    for (int c = 0; c < this->code_candidates_.size(); c++) {
        // Loop over all decoders
        for (int i = 0; i < this->decoders_.size(); i++) {
            std::cout << "Decoding with decoder #" << i << std::endl;

            cv::Mat image_candidate = util::crop(this->image_, code_candidates_[c].rotrect);
            int ret = this->decoders_[i]->Decode(image_candidate, &code_candidates_[c]);
            if (ret == RET_SUCCESS) {
                std::cout << " Barcode found : " << code_candidates_[c].data << std::endl;
                this->codes_.push_back(code_candidates_[c]);
            }
        }
    }

    return RET_NONE_FOUND;
}

} //END NAMESPACE openbarcode
