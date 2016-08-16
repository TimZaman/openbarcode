/**
--------------------------------------------------------------------------------
-   Module      :   decoder_barcode.cpp
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 16-AUG-2016
--------------------------------------------------------------------------------
*/

#include <iostream>

#include "libopenbarcode/toolkit/utils_opencv.h"
#include "libopenbarcode/toolkit/utils_general.h"

#include "libopenbarcode/decoder_barcode.h"

namespace openbarcode {

DecoderBarcode::DecoderBarcode(Options * opts) : Decoder(opts) {
    std::cout << "DecoderBarcode::DecoderBarcode()" << std::endl;
}

DecoderBarcode::~DecoderBarcode(){
    std::cout << "DecoderBarcode::~DecoderBarcode()" << std::endl;
}

int DecoderBarcode::DecodeBinary(cv::Mat image, openbarcode::code * current_code) {
    std::cout << "DecoderBarcode::DecodeBinary()" << std::endl;
    return RET_EPIC_FAIL;
}

int DecoderBarcode::Decode(cv::Mat image, openbarcode::code * current_code) {
    std::cout << "DecoderBarcode::Decode()" << std::endl;

    // Convert to grayscale
    cvtColor(image, image, cv::COLOR_BGR2GRAY); //a sad slow maybe

    bool debugstripecode = true;

    //Collapse the barcode
    cv::Mat matBarcode1D, matBarcode1Dmax;
    cv::reduce(image, matBarcode1D, image.cols < image.rows ? 1 : 0, CV_REDUCE_AVG);
    cv::reduce(image, matBarcode1Dmax, image.cols < image.rows ? 1 : 0, CV_REDUCE_MIN);

    //Make it twice as wide
    static const double scale_barcode_for_readout = 2.0;
    if (matBarcode1D.cols > matBarcode1D.rows) { 
        resize(matBarcode1D, matBarcode1D, cv::Size(), scale_barcode_for_readout, 1);
    } else { 
        resize(matBarcode1D, matBarcode1D, cv::Size(), 1, scale_barcode_for_readout);
    }

    //Binarize
    util::autoClipBrighten(matBarcode1D, 0.06, 0.94);
    util::autoClipBrighten(matBarcode1Dmax, 0.06, 0.94);
    
//    if(debugstripecode){
//        imwrite("/Users/tzaman/Desktop/bc/bc2D_" + std::to_string(i) + ".tif", image);
//        imwrite("/Users/tzaman/Desktop/bc/bc1D_" + std::to_string(i) + ".tif", matBarcode1D);
//        imwrite("/Users/tzaman/Desktop/bc/bc1Dmax_" + std::to_string(i) + ".tif", matBarcode1Dmax);
//    }

    //Test three different thresholds to account for different amounts of ink (little ink/a lot of ink/etc)
    int thresholds[]={165, 150, 180, 120, 100, 200, 215};//THIS LIST IS IN ORDER OF BEST PERFORMING TRESHOLDS!
    //int thresholds[]={150};
    bool found = false;
    for (int j = 0; j < 7; j++) {
        //cout << "j=" << j << endl;
        cv::Mat matBarcode1Dthres, matBarcode1Dmaxthres;
        threshold(matBarcode1D, matBarcode1Dthres, thresholds[j], 255, cv::THRESH_BINARY);
        threshold(matBarcode1Dmax, matBarcode1Dmaxthres, thresholds[j], 255, cv::THRESH_BINARY);

        if(debugstripecode){
            imwrite("/Users/tzaman/Desktop/bc/bc1D_t" + std::to_string(thresholds[j]) + ".tif", matBarcode1Dthres);
            imwrite("/Users/tzaman/Desktop/bc/bc1D_t" + std::to_string(thresholds[j]) + "m.tif", matBarcode1Dmaxthres);
        }

        int ret = DecodeBinary(matBarcode1Dthres, current_code);
        if (ret == RET_SUCCESS ) {
            found = true;
            break;
        }

        // Try again only taking the max (for a shitty printer) @TODO(tzaman): is this useful?
        ret = DecodeBinary(matBarcode1Dmaxthres, current_code);
        if (ret == RET_SUCCESS ) {
            found = true;
            break;
        }
    }

    if (!found) {
        return RET_NONE_FOUND;
    }

// @TODO(tzaman): where to put the below? note the "**" part only occurs with c39. Other barcode-based codes have the same issue though
//  .. so maybe make a function to check for doubles and extract those?
//    //Make sure double barcode readout does not happen i.e.: '10007**10008', so split that up
//    std::vector<std::string> split_codes;
//    split_codes = util::split(codeNow.str, "**");
//    if (split_codes.size()>1){
//        std::cout << "Found multiple split barcodes (**) from readout:'" << codeNow.str << "'" << std::endl;
//    }
//    for (int i = 0; i < split_codes.size(); i++) {
//        std::cout << "\t Pushing out (sub)barcode #" << i << "=" << split_codes[i] << std::endl;
//        stripeCode code_now_sub = codeNow; // Copy its contents
//        code_now_sub.str =  split_codes[i]; // And change the string
//        vecStripecodes.push_back(code_now_sub); // And push out
//    }

//    if (debugstripecode) {
//        imwrite("/Users/tzaman/Desktop/bc/_" + codeNow.str + ".tif", image);
//        cv::Mat matBarcodeFull = matImage.clone();
//        util::rectangle(matBarcodeFull, rotRectBarcode, Scalar(50, 255, 50), 5);
//        for (int j = 0; j < stripesVerified.size(); j++) {
//            cv::circle(matBarcodeFull, stripesVerified[j].center, 5, Scalar(50, 50, 255), 1, CV_AA, 0);
//        }
//        cv::imwrite("/Users/tzaman/Desktop/bc/_matBarcodeFull.tif", matBarcodeFull);
//    }

    return RET_SUCCESS;
}

} //END NAMESPACE openbarcode
