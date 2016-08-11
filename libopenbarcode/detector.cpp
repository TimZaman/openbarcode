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
    // std::cout << "Detector::Detector()" << std::endl;
    decoders_.push_back(dc);
}

Detector::Detector(Options *opts, std::vector< Decoder * > decoders) : opts_(opts), decoders_(decoders){
    //std::cout << "Detector::Detector()" << std::endl;
}

Detector::~Detector(){
    // std::cout << "Detector::~Detector()" << std::endl;

}

void Detector::setImage(cv::Mat image) {
    // std::cout << "Detector::setImage()" << std::endl;
    this->image_ = image;
    code_candidates_.clear();
}

int Detector::Detect() {
    // std::cout << "Detector::Detect()" << std::endl;
    std::cerr << " please override this function" << std::endl;
    return RET_NONE_FOUND;
}

cv::Mat Detector::drawAllCandidates(){
    // std::cout << "Detector::drawAllCandidates()" << std::endl;
    cv::Mat mat_draw = this->image_.clone();
    for (int c = 0; c < this->code_candidates_.size(); c++) {
        util::rectangle(mat_draw, this->code_candidates_[c].rotrect, cv::Scalar(50, 50, 255), 1);
        //putText(Mat& img, const string& text, Point org, int fontFace, double fontScale, Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false )
    }
    for (int c = 0; c < this->codes_.size(); c++) {
        util::rectangle(mat_draw, this->codes_[c].rotrect, cv::Scalar(0, 255, 0), 1);
    }
    return mat_draw;
}


int Detector::Decode() {
    // std::cout << "Detector::Decode()" << std::endl;
    // Loop over all candidates
    for (int c = 0; c < this->code_candidates_.size(); c++) {
        // Loop over all decoders
        for (int i = 0; i < this->decoders_.size(); i++) {
            //std::cout << "Decoding with decoder #" << i << std::endl;
            cv::Mat image_candidate = util::crop(this->image_, code_candidates_[c].rotrect);
            int ret = this->decoders_[i]->Decode(image_candidate, &code_candidates_[c]);
            if (ret == RET_SUCCESS) {
                // std::cout << " Barcode found : " << code_candidates_[c].data << std::endl;
                this->codes_.push_back(code_candidates_[c]);
            }
        }
    }

    // Remove dupes, compare the same found codes through rectangle similarity
    const double DUPLICATE_OVERLAP_THRESHOLD = 0.9;
    std::vector< int > duplicate_indices;
    for (int ci1 = 0; ci1 < this->codes_.size(); ci1++) {
        for (int ci2 = ci1+1; ci2 < this->codes_.size(); ci2++) {
            double similarity = util::rectangleSimilarity(this->codes_[ci1].rotrect.boundingRect(), this->codes_[ci2].rotrect.boundingRect());
            if (similarity > DUPLICATE_OVERLAP_THRESHOLD) {
                duplicate_indices.push_back(ci2);
            }
        }
    }
    // Erase Duplicate-Duplicates
    std::sort( duplicate_indices.begin(), duplicate_indices.end() );
    duplicate_indices.erase( unique( duplicate_indices.begin(), duplicate_indices.end() ), duplicate_indices.end() );
    // Erase Duplicates
    for (int di = duplicate_indices.size()-1; di >=0; di--) {
        this->codes_.erase(this->codes_.begin()+duplicate_indices[di]);    
    }
    
    return RET_NONE_FOUND;
}

} //END NAMESPACE openbarcode
