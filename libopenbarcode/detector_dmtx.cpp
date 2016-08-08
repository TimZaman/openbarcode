/**
--------------------------------------------------------------------------------
-   Module      :   detector_dmtx.cpp
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 8-AUG-2016
--------------------------------------------------------------------------------
*/

#include <iostream>
#include <string>
#include <vector>

#include "libopenbarcode/toolkit/sort.h"
#include "libopenbarcode/toolkit/utils_opencv.h"
#include "libopenbarcode/toolkit/utils_general.h"

#include "libopenbarcode/detector_dmtx.h"

using namespace std;
using namespace cv;

namespace openbarcode {

DetectorDmtx::DetectorDmtx(Options * opts, Decoder * dc) : Detector(opts, dc) {
    std::cout << "DetectorDmtx::DetectorDmtx()" << std::endl;
}

DetectorDmtx::DetectorDmtx(Options * opts, std::vector< Decoder * > decoders) : Detector(opts, decoders) {
    std::cout << "DetectorDmtx::DetectorDmtx()" << std::endl;
}

DetectorDmtx::~DetectorDmtx() {
    std::cout << "DetectorDmtx::~DetectorDmtx()" << std::endl;

}

int DetectorDmtx::Detect() {
    std::cout << "DetectorDmtx::Detect()" << std::endl;

    // @TODO(tzaman): Convert 16bit input to 8bit
    const int dpi = 300;
    const double barcode_width_inch_min = 0.10; // 0.15
    const double barcode_width_inch_max = 1.78; // 0.40
    const int bin_thres = 100; // 40 to 140
    const int min_pixel_area = 20*20;

    std::vector< int > binary_thresholds;
    binary_thresholds.push_back(70);
    binary_thresholds.push_back(100);
    binary_thresholds.push_back(130);


    const double squareness_threshold = 0.1; //f.e. 0.1=10% deviation aspect ratio width/height
    const double barcode_width_inch = 0.6; //DMTX oslo 0.48inch dmtx  (1dim)
    const double bc_margin_extra_inch = 0.05; //add margin to final crop (in inches)

    const bool debugbarcode = false; //FOR PRODUCTION PUT TO FALSE

    Mat matImageK;
    cvtColor(this->image_, matImageK, cv::COLOR_BGR2GRAY);

    // Try dilation (tzaman): dilation works well on bad barcodes, but can mess up normal ones
    //int erosion_type = MORPH_ELLIPSE;
    //int erosion_size = 1; //default 2 (for size 5x5)
    //Mat element = getStructuringElement( erosion_type,
    //                                   Size( 2*erosion_size + 1, 2*erosion_size+1 ),
    //                                   Point( erosion_size, erosion_size ) );
    //erode(matImageK, matImageK, element);// cv::Mat::zeros(5,5,CV_8UC1));

    // Blur it to supress noise
    Mat matImageKblur;
    cv::GaussianBlur(matImageK, matImageKblur, cv::Size(0, 0), 1.0);

    // Find the contours in the image
    std::vector< std::vector< cv::Point > > contours;

    // Perform three thresholds (way faster, stable and scale invariant than adaptive)
    for (int ti = 0; ti < binary_thresholds.size(); ti++) { 
        std::vector< std::vector< cv::Point > > local_contours;
        std::vector< cv::Vec4i > hierarchy;
        Mat matImageC;
        threshold(matImageKblur, matImageC, binary_thresholds[ti], 255, THRESH_BINARY);
        findContours(matImageC, local_contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
        
        // Append local contour
        contours.insert(std::end(contours), std::begin(local_contours), std::end(local_contours));
    }

    // Note we can get very similar contours here but that's really no problem.

    // Calculate size to add to rect in pixels
    cv::Point bc_ptadd(bc_margin_extra_inch * dpi * 0.5, bc_margin_extra_inch * dpi * 0.5);
    cv::Size bc_szadd(bc_margin_extra_inch * dpi, bc_margin_extra_inch * dpi);

    // Calculate thresholds
    // Aspect ratio thresholds
    double ar_th_low  = 1.0 - squareness_threshold;
    double ar_th_high = 1.0 + squareness_threshold;

    // Barcode size thresholds
    double bc_th_low  = dpi * barcode_width_inch_min;
    double bc_th_high = dpi * barcode_width_inch_max;

    // Iterate the contours
    for (int i = 0; i < contours.size(); i++) { 
        double a = contourArea(contours[i], false);  // Find the area of contour
        if (a < min_pixel_area) {
            continue;
        }
    
        if (contours[i].size() < 4) {
            continue;
        }
        
        RotatedRect rotrect_candidate = util::minAreaSquare(contours[i]);

        //First check for squareness
        int w = rotrect_candidate.size.width;
        int h = rotrect_candidate.size.height; 
        double ar = double(w) / double(h); //Aspect Ratio
        if (ar < ar_th_low || ar > ar_th_high) { //If not square
            continue;
        }

        //Distinguish by size
        int avg = (w + h) * 0.5; //Average dimension of the square (w and h avg)
        if (avg < bc_th_low || avg > bc_th_high) {
            continue;
        }

        //Put the barcodes as upright a we found them
        rotrect_candidate = util::fixRotatedRect(rotrect_candidate);

        double rotangle = rotrect_candidate.angle;

        cv::Rect boundRect = rotrect_candidate.boundingRect();
        boundRect = util::constrainRectInSize(boundRect, matImageK.size());

        if (debugbarcode) {
            cv::Mat image = this->image_.clone();
            util::rectangle(image, rotrect_candidate, Scalar(0, 255, 0), 1);

            //util::rectangle(image, rSquare, Scalar(0,0,255), 1);

            /*for(int j=0;j<convex_hull.size();j++){
                cout << " " << j << " " << convex_hull[j] << endl;
                int j2 = j+1 == convex_hull.size() ? 0 : j+1; //Workaround to connect last to first
                line(image, convex_hull[j], convex_hull[j2], Scalar(255,0,0), 1);
            }*/

            //rectangle(image, boundRect, Scalar(255,0,0), 1);
            //drawContours( matImage, contours, 0, Scalar(255), CV_FILLED, 8, hierarchy ); // Draw the largest contour using previously stored index.
            //imshow("rectangles", image);
            imwrite("/Users/tzaman/Desktop/bc/" + std::to_string(i) + "_dmtx_rect.png", image(boundRect));
        }

        /*
        // Histogram correspondance params
        const double hist_thres = 0.60; //f.e. 0.75=75% histgram comparison (with band-stop histogram filter)
        const int histSize = 16; // from 0 to 255
        const float range[] = { 0, 256 } ; // the upper boundary is exclusive
        const float * histRange = { range };
        const bool uniform = true;
        const bool accumulate = false;
        // Define the band-stop filter (that weights the center, gray, low and the whites and blacks high)
        const double idealhist[16]={1.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0};
        Mat matHist = matImageK(boundRect).clone();

        util::autoClipBrighten(matHist, 0.05, 0.95);

        //Check if histogram is any good, should have two distinct peaks.
        Mat hist32;
        calcHist(&matHist, 1, 0, Mat(), hist32, 1, &histSize, &histRange, uniform, accumulate);

        //Normalize such that the area is 1 (not the peaks!)
        double histarea = 0;
        for (int i= 0 ; i < 16; i++) {
            histarea += hist32.at<float>(i);
            //cout << hist32.at<float>(i) << " ";
        }
        //cout << endl << "histarea=" << histarea << endl;

        //Account for the histogram area, set cum to 1.
        for (int i = 0; i < 16; i++) {
            hist32.at<float>(i) *= (1.0 / histarea);
        }
        */

        //cout << hist32 << endl;

        //Compare with the ideal histogram and compute score
        //Compute cross correction
        //double cumscore=0;
        //for (int i=0;i<16;i++){
        //  cumscore += idealhist[i]*hist32.at<float>(i);
        //}
        ////cout << "cumscore=" << cumscore << endl;
        //if (debugbarcode){
        //  imwrite("/Users/tzaman/Desktop/bc/" + boost::lexical_cast<string>(i)+"_dmtx_rect_hist_c" + std::to_string((int)round(cumscore*100)) +".png", matHist);
        //}
        //A cumscore value of 1 is the maximum that can be attained.
        //if (cumscore < hist_thres){
            //std::cout << "histogram cumscore too low (" << cumscore << " / " << hist_thres << "). rejecting candidate " << i <<"." << std::endl;
            //continue;
        //}

        //See if we can read it purely..

        Mat matPureCrop = util::crop(matImageK, rotrect_candidate);

        //Check if the size is still in a normal range
        if (matPureCrop.total() < 50) {
            continue;
        }

        //imwrite("dmtx_purecrop.png", matPureCrop);

        openbarcode::code code_candidate;
        code_candidate.rotrect = rotrect_candidate;
        this->code_candidates_.push_back(code_candidate);
    }

    return RET_SUCCESS;
}

} /* namespace openbarcode */
