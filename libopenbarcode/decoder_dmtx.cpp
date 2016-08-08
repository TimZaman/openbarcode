/**
--------------------------------------------------------------------------------
-   Module      :   decoder_dmtx.cpp
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 8-AUG-2016
--------------------------------------------------------------------------------
*/

#include <iostream>
#include <string>
#include <vector>

#include "libopenbarcode/toolkit/utils_opencv.h"
#include "libopenbarcode/toolkit/utils_general.h"

#include "libopenbarcode/decoder_dmtx.h"

// @TODO(tzaman) make own implementation without dmtx
#include <dmtx.h> 
//using namespace datamatrix;

namespace openbarcode {

DecoderDmtx::DecoderDmtx(Options * opts) : Decoder(opts) {
    std::cout << "DecoderDmtx::DecoderDmtx()" << std::endl;

}

DecoderDmtx::~DecoderDmtx() {
    std::cout << "DecoderDmtx::~DecoderDmtx()" << std::endl;

}

int DecoderDmtx::Decode(cv::Mat image, openbarcode::code * current_code) {
    std::cout << "DecoderDmtx::Decode()" << std::endl;

    std::string bcString = "";
    cv::Mat matImageK_orig;
    if (image.channels() == 3) {
        cv::cvtColor(image, matImageK_orig, cv::COLOR_BGR2GRAY);
    } else {
        matImageK_orig = image;//.clone();
    }

    // Enhange image
    cv::Mat matImageK = matImageK_orig.clone();

    // Auto enhance darkness and brightness
    util::autoClipBrighten(matImageK, 0.10, 0.90);
    
    const double min_border_validity_percentage = 0.85;

    // Extract the width and height of the barcode.
    // Currently we only use 10, 12, 14.
    int numsizes = 4;
    int sizes[4] = {10, 12, 14, 16};

    float stddevs[numsizes];
    for (int i = 0; i < numsizes; i++) {
        int w = sizes[i];
        cv::Mat matBits;
        cv::resize(matImageK, matBits, cv::Size(w, w), 0, 0, cv::INTER_AREA); 

        cv::Scalar mean,stddev;
        meanStdDev(matBits, mean, stddev);
        stddevs[i] = stddev[0];
        //cout << "mean=" << mean << " stddev=" << stddev << endl;
    }

    //Find highest one
    float max = 0;
    int imax = 0;
    for (int i = 0; i < numsizes; i++){
        if (stddevs[i] > max) {
            imax = i;
            max = stddevs[i];
        }
    }

    //We found the width
    int width = sizes[imax];
    int height = width;

    //Reconstruct the image for sampling
    cv::Mat matBits;
    resize(matImageK, matBits, cv::Size(width, height), 0, 0, cv::INTER_AREA); 

    //Finally, check the barcode's orientation, white patch should be in the top right.
    if (matBits.at<uchar>(0, 0) > 128){ //Its in the top left
        util::rot90(matBits, 1); //90CW
    } else if (matBits.at<uchar>(height - 1, width - 1) > 128){ //Its in the bottom right
        util::rot90(matBits, 2); //90CCW
    } else if (matBits.at<uchar>(height - 1,0) > 128){ //Its in the bottom left
        util::rot90(matBits, 3); //180
    } //else: good place already, leave it.

    if (0){
        cv::imwrite("/Users/tzaman/Desktop/bc/matBitsRot.png", matBits);
        //std::cout << "plz press key" << std::endl;
        //char a;
        //cin >> a;
    }

    /*
    bool vals[144]= //this is actually an external 12x12 matrix that says 'test'
    {1,0,1,0,1,0,1,0,1,0,1,0,
     1,0,1,1,0,0,1,1,0,0,1,1,
     1,1,0,0,1,0,1,0,1,1,1,0,
     1,1,1,0,0,0,0,1,0,1,0,1,
     1,0,1,1,0,0,1,0,0,0,1,0,
     1,1,0,0,1,0,0,0,0,0,0,1,
     1,1,1,1,0,1,0,0,0,0,0,0,
     1,0,0,1,0,1,0,1,0,1,0,1,
     1,1,1,0,1,1,0,1,1,1,1,0,
     1,0,1,0,1,0,0,0,0,1,0,1,
     1,1,1,0,0,1,1,1,0,0,1,0,
     1,1,1,1,1,1,1,1,1,1,1,1};

    bool vals[100]=
        {1,0,1,0,1,0,1,0,1,0,
         1,1,1,0,0,0,1,0,1,1,
         1,0,0,0,1,0,1,0,1,0,
         1,1,0,0,0,0,1,1,1,1,
         1,0,1,0,1,0,0,0,0,0,
         1,0,0,0,1,1,0,1,1,1,
         1,1,0,0,0,0,0,0,0,0,
         1,1,0,1,1,0,1,1,0,1,
         1,0,1,0,0,1,1,0,0,0,
         1,1,1,1,1,1,1,1,1,1};*/


    std::vector<int> thresholds;
    thresholds.push_back(100); //Attempt a fixed threshold for value 100 [0:255]
    //Find a dynamic threshold for badly printed targets.
    //The strategy used here is finding a threshold so that the entire
    //'connected edge' (bottom left) is filled (black). So find the minimum black
    //value therein
    int minVal = 255; // Initialize with highest value
    for (int x = 0; x < matBits.cols; x++) {
        int valNow = matBits.at<uchar>(x, matBits.rows - 1);
        if (valNow < minVal) {
            minVal = valNow;
        }
    }
    for (int y = 0; y < matBits.rows; y++) {
        int valNow = matBits.at<uchar>(0, y);
        if (valNow < minVal) {
            minVal = valNow;
        }
    }
    //cout << "minVal=" << minVal << endl;
    //Now finally add a few points for a little margin.
    minVal = minVal + 5;
    thresholds.push_back(minVal);

    int num_decode_fails = 0;
    int num_ = 0;

    



    for (int i = 0; i < thresholds.size(); i++) {
        cv::Mat matBitsThres;
        cv::threshold(matBits, matBitsThres, thresholds[i], 255, cv::THRESH_BINARY);

        //Verify the L-shape and the timing
        
        int validity_points = 0;
        int validity_points_max = matBitsThres.cols * 2 + matBitsThres.rows * 2 - 4; //length of entire edge
        //Vertical Check
        for (int h = 0; h < matBitsThres.rows; h++) {
            if(matBitsThres.at<uchar>(h, 0) == 0) { //Continuous part of L-shape
                validity_points++;
            }
            if((matBitsThres.at<uchar>(h, matBitsThres.cols - 1) == 0) == h % 2) { // Alternating test
                validity_points++;
            } 
        }

        //Horizontal check
        for (int w = 0; w < matBitsThres.cols; w++) {
            if(matBitsThres.at<uchar>(matBitsThres.rows-1, w) == 0) {//Continuous part of L-shape
                validity_points++;
            }
            if((matBitsThres.at<uchar>(0, w) == 255) == w % 2) { // Alternating test
                validity_points++;  
            } 
        }

        validity_points -= 4; //Subtract redundant points (4 corners)
        if (validity_points < validity_points_max * min_border_validity_percentage) {
            //std::cout << " rejecting validity (" << validity_points << "/" << validity_points_max << ")" << std::endl;
            current_code->rejection_counter[REJECT_DEC_DMTX_TOO_FEW_TIMING_POINTS]++;
            continue;
        }

        //imwrite("/Users/tzaman/Desktop/bc_" + std::to_string(thresholds[i]) + ".tif",matBitsThres); //@TODO REMOVE ME
        std::string string_libdmtx = doDmtxDecode(matBitsThres, 500);
        if (!string_libdmtx.empty()) {
            current_code->data = string_libdmtx;
            return RET_SUCCESS;
        } else {
            current_code->rejection_counter[REJECT_DEC_DMTX_DECODING_FAILED]++;
        }

    }

    return RET_NONE_FOUND;
}

std::string DecoderDmtx::doDmtxDecode(const cv::Mat &matImageIn, long timeout_ms) {
    std::cout << "doDmtxDecode(..," << timeout_ms << ")" << std::endl;
    std::string string_libdmtx;

    int size_idx = getSizeIdxFromSymbolDimension(matImageIn.cols, matImageIn.rows);

    DmtxMessage * msg = dmtxMessageCreate(size_idx, DmtxFormatMatrix);

    int idx = 0;
    for (int h = 1; h < matImageIn.rows - 1; h++) {
        for (int w = 1; w < matImageIn.cols - 1; w++) {
            msg->array[idx] = matImageIn.at<uchar>(h,w) == 255 ? DmtxModuleOff : DmtxModuleOnRGB;
            msg->array[idx] |= DmtxModuleAssigned;
            idx++;
        }
    }

    dmtxDecodePopulatedArray(size_idx, msg, -1);

    if ((msg != NULL) && (msg->output[0] != NULL)) {
        string_libdmtx = std::string(reinterpret_cast<char*>(msg->output));
        std::cout << " string_libdmtx = " << string_libdmtx << std::endl;
        dmtxMessageDestroy(&msg);
    }

    std::cout << "END doDmtxDecode()" << std::endl;
    return string_libdmtx;
}

} //END NAMESPACE openbarcode
