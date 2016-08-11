/**
--------------------------------------------------------------------------------
-   Module      :   detector_barcode.cpp
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 5-AUG-2016
--------------------------------------------------------------------------------
*/

#include <iostream>
#include <string>
#include <vector>

#include "libopenbarcode/toolkit/sort.h"
#include "libopenbarcode/toolkit/utils_opencv.h"
#include "libopenbarcode/toolkit/utils_general.h"

#include "libopenbarcode/detector_barcode.h"


using namespace std;
using namespace cv;

namespace openbarcode {

DetectorBarcode::DetectorBarcode(Options * opts, Decoder * dc) : Detector(opts, dc) {
    // std::cout << "DetectorBarcode::DetectorBarcode()" << std::endl;
}

DetectorBarcode::DetectorBarcode(Options * opts, std::vector< Decoder * > decoders) : Detector(opts, decoders) {
    // std::cout << "DetectorBarcode::DetectorBarcode()" << std::endl;
}

DetectorBarcode::~DetectorBarcode() {
    // std::cout << "DetectorBarcode::~DetectorBarcode()" << std::endl;

}

int DetectorBarcode::Detect() {
    // std::cout << "DetectorBarcode::Detect()" << std::endl;

    assert(this->image_.data != NULL);

    bool debugstripecode = true; //@TODO MAKE SURE TO SET ME TO FALSE IN PRODUCTION
    bool useAdaptiveThersholding = true;

    int dpi = 400; //this works well for all scales and sizes..

    Mat matImageK;
    cvtColor(this->image_, matImageK, cv::COLOR_BGR2GRAY);
    cv::Mat matThres;

    // VARIABLES //
    double bar_height_mm_min = 3.7; //[7.5mm=our NMNH c39] [10.7mm=NMNH cover c39]
    double bar_height_mm_max = 20;

    double bar_ar_min = 4;
    double bar_ar_max = 110;

    int min_characters = 5; //minimum characters in barcode string

    double bar_dist_group_mm_max = 9.0; //Maximum distance between any grouped bar to be part of the bar group

    // COMPUTE //
    double bar_height_px_min = bar_height_mm_min/25.4*dpi;
    double bar_height_px_max = bar_height_mm_max/25.4*dpi;

    double bar_area_px_min = bar_height_px_min*(bar_height_px_min*1.0/bar_ar_max);

    //Dont allow the area to be less than 1px row
    bar_area_px_min = bar_area_px_min < bar_height_px_min ? bar_height_px_min : bar_area_px_min;

    double bar_area_px_max = bar_height_px_max*(bar_height_px_max*1.0/bar_ar_min);

    double bar_dist_group_px_max = bar_dist_group_mm_max/25.4*dpi;

    if (useAdaptiveThersholding) {
        //int AT_blocksize = dpi*0.05; 
        int AT_blocksize = bar_height_px_min*0.5;
        int AT_iseven=AT_blocksize%2;
        AT_blocksize += 1+AT_iseven; //Makes sure the blocksize is an even number
        //cout << "AT_blocksize=" << AT_blocksize << endl;
        adaptiveThreshold(matImageK, matThres, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, AT_blocksize, 20);
    } else {
        threshold(matImageK, matThres, 127, 255, THRESH_BINARY_INV);
    }

    if (debugstripecode) {
        //cout << "dpi=" << dpi << endl;
        imwrite("/Users/tzaman/Desktop/bc/matImage.tif", this->image_);
        imwrite("/Users/tzaman/Desktop/bc/matThres.tif", matThres);
    }

    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( matThres, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );
    
    //cout << "contours.size()=" << contours.size() << endl;
    
    if (contours.size() == 0) {
        // cout << "No contours found." << endl;
        return RET_NONE_FOUND;
    }

    //RANSAC vars
    int min_inliers = (min_characters+2)*5*0.75; //+2 (start&stop), *5 (stripes per char), *0.x (margin)
    double max_px_dist = (bar_height_px_min+bar_height_px_max)*0.5*0.05; //Maximum distance from RANSAC line to a point

    vector<RotatedRect> stripeCandidates;
    for(int i = 0; i >= 0; i = hierarchy[i][0] ) {
        double cArea = contourArea( contours[i],false); 
        if (cArea < bar_area_px_min*0.5){
            continue;
        }
        if (cArea > bar_area_px_max){
            continue;
        }
        //cout << "[" << i << "]" << " cArea=" << cArea << endl;

        RotatedRect rotRect= minAreaRect(contours[i]);
        double ar = max(double(rotRect.size.width),double(rotRect.size.height)) / min(double(rotRect.size.width),double(rotRect.size.height));

        if (ar < bar_ar_min){
            continue;
        }
        if (ar > bar_ar_max){
            continue;
        }

        double width = std::min(rotRect.size.width, rotRect.size.height);
        double height = std::max(rotRect.size.width, rotRect.size.height);

        //Check the length
        if (height < bar_height_px_min) {
            //Stripe too small
            continue;
        }

        if (height > bar_height_px_max ) {
            //Stripe too long
            continue;
        }

        Rect rCrop = boundingRect(contours[i]);

        //Below parameter is dynamic, plz note
        double min_area_fill = 0.15;// = 0.25 ;// 0.4 means 40% of the bounding rectangle of the contour needs to be filled
        //The min_area_fill threshold should be dependent on the width in pixels, because there's more noise in thinner ones
        if (width < 3){
            min_area_fill = 0.05;
        } else if (width < 5){
            min_area_fill = 0.10;
        }


        //Check if the rectangle is actually filled well
        int fullarea = rCrop.area();

        
        if ( (double(cArea) / double(fullarea)) < min_area_fill) {
            continue;
        }

        //cout << i << " fullarea=" << fullarea << " carea=" << cArea << endl;

        if (debugstripecode) {
            imwrite("/Users/tzaman/Desktop/seg/" + std::to_string(i) +  ".tif", matImageK(rCrop));
        }
        stripeCandidates.push_back(rotRect);
    }


    if (debugstripecode) {
        Mat matBarcodeFull = this->image_.clone();
        for (int j = 0; j < stripeCandidates.size(); j++){
            util::rectangle(matBarcodeFull, stripeCandidates[j], cv::Scalar(255, 0, 0), 2);
        }
        imwrite("/Users/tzaman/Desktop/bc/_candidates.tif", matBarcodeFull);
    }


    //cout << "stripeCandidates.size()=" << stripeCandidates.size() << endl;

    if (stripeCandidates.size() < min_inliers) {
        // cout << "Code 39 did not find enough bars to accurately make a code." << endl;
        return RET_NONE_FOUND;
    }
    
    std::vector<Point> vecPtRectCenter = util::vecrotrect2vecpt(stripeCandidates);
    std::vector<std::vector<int> > vecGroupIdxs = util::groupPoints(vecPtRectCenter, bar_dist_group_px_max, min_inliers);
    //std::vector<std::vector<cv::Point> > vecGroupPts(vecGroupIdxs.size());
    std::vector<std::vector<cv::RotatedRect> > vecGroupRects(vecGroupIdxs.size());

    //Relate indexes to points and add to group vector
    for (int i = 0; i < vecGroupIdxs.size(); i++) {
        //vecGroupPts[i].resize(vecGroupIdxs[i].size());
        vecGroupRects[i].resize(vecGroupIdxs[i].size());
        for (int j = 0; j < vecGroupIdxs[i].size(); j++){
            //cout << i << "," << j << endl;
            //vecGroupPts[i][j] = vecPtRectCenter[vecGroupIdxs[i][j]];
            vecGroupRects[i][j] = stripeCandidates[vecGroupIdxs[i][j]];
        }
    }

    //Draw all groups
    //if(debugstripecode){
    //  for (int i=0; i<vecGroupPts.size(); i++){
    //      Mat matGroup = matImage.clone();
    //      for (int j=0; j<vecGroupPts[i].size(); j++){
    //          circle(matGroup, vecGroupPts[i][j], 5, Scalar(255,0,255), 1, CV_AA,0);              
    //      }
    //      imwrite("/Users/tzaman/Desktop/bc/_group_" + std::to_string(i) + ".tif", matGroup);
    //  }
    //}
    //exit(-1);

    //cout << "vecGroupPts.size()=" << vecGroupPts.size() << endl;
    //Erase small groups
    //for (int i=vecGroupPts.size()-1; i>=0; i--){
    //  if (vecGroupPts[i].size() < min_inliers){
    //      //Skipping group, too small.
    //      vecGroupIdxs.erase(vecGroupIdxs.begin()+i);
    //      vecGroupPts.erase(vecGroupPts.begin()+i);
    //  }
    //}
    //cout << "vecGroupPts.size()=" << vecGroupPts.size() << endl;

    if (vecGroupIdxs.size() == 0) {
        // cout << "Code 39 failed to ransac bars in a line." << endl;
        return RET_NONE_FOUND;
    }

    //Now cycle over the groups
    vector<vector<int> > vecVecInlierIdx;
    vector<Vec4f> vecLines;
    vector<int> vecFromGroup; //Keeps track of which group the vecvecInlierIdx belongs to
    for (int i = 0; i < vecGroupRects.size(); i++) {
        Ransac(vecGroupRects[i], min_inliers, max_px_dist, vecVecInlierIdx, vecLines, this->image_);
        vecFromGroup.resize(vecVecInlierIdx.size(), i);
    }

    if (vecLines.size() == 0) {
        // cout << "Code 39 failed to ransac bars in a line." << endl;
        return RET_NONE_FOUND;
    } else {
        //cout << "Code39 ransac succesfull" << endl;
    }

    //for (int i=0; i<vecGroupIdxs.size(); i++){
    //  cout << "Group " << i << " (" << vecGroupIdxs[i].size() << ") : ";
    //  for (int j=0; j<vecGroupIdxs[i].size(); j++){
    //      cout << vecGroupIdxs[i][j] << " ";
    //  }
    //  cout << endl;
    //}


    //Convert back vecVecInlierIdx to original indices
    for (int i=0; i<vecVecInlierIdx.size(); i++){
        //cout << "vecVecInlierIdx[" << i << "] is from group " << vecFromGroup[i] << endl;
        for (int j=0; j<vecVecInlierIdx[i].size(); j++){
            //cout << " " << vecVecInlierIdx[i][j] << " -> " << vecGroupIdxs[vecFromGroup[i]][vecVecInlierIdx[i][j]] << endl;
            vecVecInlierIdx[i][j] = vecGroupIdxs[vecFromGroup[i]][vecVecInlierIdx[i][j]];
        }
    }


    for (int i=0; i < vecLines.size(); i++){
        int numpts = vecVecInlierIdx[i].size();
        // cout << "Potential barcode #" << i << " with " << numpts << " points." << endl;

        
        //double angle=atan2(vecLines[i][1],vecLines[i][0])*180/M_PI; //For some reason it clips from [-90,90]
        double angle_rad = atan2(vecLines[i][1],vecLines[i][0]); //For some reason it clips from [-90,90]
        double angle_deg = angle_rad*180.0/M_PI;
        //cout << " angle_deg=" << angle_deg << endl;

        vector<double> bar_heights(numpts);
        vector<double> bar_widths(numpts);
        vector<double> coords_x(numpts);
        //Loop over all found and ransac-verified stripes in this barcode
        vector<cv::RotatedRect> stripesVerified(numpts);
        for (int j=0; j < numpts; j++){
            //cout << vecVecInlierIdx[i][j] << endl;
            //cout << "checking out stripecandidate[" << vecVecInlierIdx[i][j] << "] #" << vecVecInlierIdx[i][j] << endl;
            stripesVerified[j] = stripeCandidates[vecVecInlierIdx[i][j]];
            double dim_smallest = min(stripesVerified[j].size.width, stripesVerified[j].size.height); //For rotation invariance
            double dim_tallest  = max(stripesVerified[j].size.width, stripesVerified[j].size.height); //For rotation invariance
            bar_heights[j] = dim_tallest;
            bar_widths[j]  = dim_smallest;

            //Rotate the points straight
            Point2f ptRot = util::rotatePoint(stripesVerified[j].center, Point(matImageK.cols, matImageK.rows), angle_rad);
            //cout << ptRot << endl;
            coords_x[j]    = ptRot.x;
        }
        
        double height_median = util::calcMedian(bar_heights);
        double width_mean = util::calcMean(bar_widths);
        //cout << "height_median=" << height_median <<" width_mean=" << width_mean << endl;

        //Find the start and end position for reading
        vector<size_t> coords_sorted_index;
        vector<double> coords_x_sorted;
        sort(coords_x, coords_x_sorted, coords_sorted_index);
        //cout << coords_x_sorted[0] << " -> " << coords_x_sorted[coords_x_sorted.size()-1] << endl;

        //Get extrema-stripes
        Point2f pt_stripe_left = stripeCandidates[vecVecInlierIdx[i][coords_sorted_index[0]]].center;
        Point2f pt_stripe_right = stripeCandidates[vecVecInlierIdx[i][coords_sorted_index[coords_sorted_index.size() - 1]]].center;
        //cout << "pt_stripe_left=" << pt_stripe_left << endl;
        //cout << "pt_stripe_right=" << pt_stripe_right << endl;

        Point2f pt_barcode_center = (pt_stripe_left + pt_stripe_right) * 0.5;
        //cout << "pt_barcode_center=" << pt_barcode_center << endl;

        //Calculate width of the barcode
        double barcode_width = util::pointDist(pt_stripe_left, pt_stripe_right);
        //cout << "barcode_width=" << barcode_width << endl;

        //Make the rotated rectangle around the barcode
        cv::RotatedRect rotrect_candidate(pt_barcode_center, Size2f(barcode_width, height_median), angle_deg);

        const double add_width_on_sides_before_decoding = 7.0; // this number will be multiplied by average bar width

        //Add margin (of a few median widths)
        rotrect_candidate.size += Size2f(width_mean * add_width_on_sides_before_decoding, 0);

        const double height_retrainer_for_collapse = 0.25;

        //Extract the barcode itself
        cv::RotatedRect rotrect_candidate_thin = rotrect_candidate;

        //Crop off some margin in thickness because we dont want to collapse the entire barcode.
        if (rotrect_candidate_thin.size.width < rotrect_candidate_thin.size.height) {
            rotrect_candidate_thin.size.width *= height_retrainer_for_collapse;
        } else {
            rotrect_candidate_thin.size.height *= height_retrainer_for_collapse;
        }

        openbarcode::code code_candidate;
        code_candidate.rotrect = rotrect_candidate_thin;

        this->code_candidates_.push_back(code_candidate);
    }

    return RET_SUCCESS;
}


void DetectorBarcode::Ransac(std::vector<cv::RotatedRect> vecRectsIn, int min_inliers, double max_px_dist, std::vector< std::vector<int> > & vecVecInlierIdx, std::vector<cv::Vec4f> & vecLines, const cv::Mat image){
    //cout << "cpRansac_barcode().." << endl;

    //This is a custom Ransac function that extracts lotsa lines
    RNG rng;

    const bool debugransac = false;

    const int minstart = 2; //Amount of randomly chosen points RANSAC will start with
    const int numiter = 10000; //Amount of iterations

    const double stripediff_min = 0.88; // 0.88=88%
    const double stripediff_max = 1.12; // 1.12=12%

    int numpts = vecRectsIn.size();

    //This next array keeps track of which points we have already used
    int incrnumsOK[numpts];
    for (int i = 0; i < numpts; i++) {
        incrnumsOK[i] = i;
    }

    for (int i = 0; i < numiter; i++) {
        
        Mat img_debug;
        if (debugransac) {
            cout << "#i=" << i << endl;
            image.copyTo(img_debug);
        }
        vector<RotatedRect> vecRectsCandidate;
        vector<int> vecRectsCandidateIdx;

        int inliers_now = minstart;
        //Make the array [0:1:numpts]
        //This array will keep track of the numbers in vecRectsIn that we use
        int incrnums[numpts];
        for (int ii = 0; ii < numpts; ii++) {
            incrnums[ii] = ii;
        }

        //Count how many we have used already
        int numcandidatesLeft = 0;
        for (int ii = 0; ii < numpts; ii++){
            if (incrnumsOK[ii] != -1) {
                numcandidatesLeft++;
            }
        }
        if (numcandidatesLeft < min_inliers) {
            break;
        }


        //Select 'minstart' amount of points
        int tries = 0;
        for(int ii = 0; ii < minstart; ii++) {
            tries++;
            //cout<<"  ii="<<ii<<endl;
            int rn = floor(rng.uniform(0., 1.)*numpts);
            //cout << "  rn=" << rn << endl;
            if ((incrnums[rn] == -1) || (incrnumsOK[rn] == -1)) { //Make sure the point we chose is unique
                ii--;  //If the point is not unique (already chosen) we select a new one
                if (tries > 5000) {
                    break;
                }
            } else {
                incrnums[rn]=-1;
                vecRectsCandidate.push_back(vecRectsIn[rn]);
                vecRectsCandidateIdx.push_back(rn);
                if (debugransac) {
                    //cout <<"  pt#"<<ii<<" (x,y)=("<<vecPtsIn[rn].x<<","<<vecPtsIn[rn].y<<")"<<endl;
                    circle(img_debug, vecRectsIn[rn].center, 12, Scalar(255,100,100), 2,CV_AA,0);
                    //namedWindow("win", 0);
                    //imshow( "win", img_debug );
                    //waitKey(10);
                }
            }
        }
        if (tries > 5000) {
            break;
        }
        
        //We have now selected a few random candidates, compute the stripeheight from those
        int stripeheight_now = 0;
        for (int s = 0; s < vecRectsCandidate.size(); s++) {
            stripeheight_now += max(vecRectsCandidate[s].size.width, vecRectsCandidate[s].size.height); //Sum it up (later divide for average)
        }
        stripeheight_now = stripeheight_now/vecRectsCandidate.size(); //This is the average
        



        //Fit the line
        Vec4f line;
        Point2f pt1,pt2;
        fitLine(cv::Mat(util::vecrotrect2vecpt(vecRectsCandidate)), line, CV_DIST_L2, 0, 0.01, 0.01);

        //Now get two points on this line
        double dd = sqrt(line[0] * line[0] + line[1] * line[1]);
        line[0] /= dd;
        line[1] /= dd;
        double t = double(image.cols + image.rows);
        pt1.x = round(line[2] - line[0] * t);
        pt1.y = round(line[3] - line[1] * t);
        pt2.x = round(line[2] + line[0] * t);
        pt2.y = round(line[3] + line[1] * t);

        /*
        if (debugransac){
            //Shows the line that passes the angle constrant
            cv::line(img_debug, pt1, pt2, Scalar(0,255,255), 10, CV_AA, 0 );
            namedWindow("win", WINDOW_NORMAL);
            Mat img_tmp;
            resize(img_debug, img_tmp, Size(), 0.05, 0.5);
            imshow( "win", img_tmp );
            waitKey(200);
            //cout << line[0] << " "<< line[1] << " "<< line[2] << " "<< line[3] << endl;
        }*/
        //For every point not in maybe_inliers we iterate and add it to the set
        for (int ii = 0; ii < numpts; ii++) {
            if ((incrnums[ii] != -1) && (incrnumsOK[ii] != -1)) { //Dont chose points already in the model
                Point2f pt0 = vecRectsIn[ii].center;
                //Calculate the distance between this point and the line (model)
                double dist = abs((pt2.x - pt1.x) * (pt1.y - pt0.y) - (pt1.x - pt0.x) * (pt2.y - pt1.y));
                dist = dist / (sqrt(pow(double(pt2.x - pt1.x), 2) + pow(double(pt2.y - pt1.y), 2)));
                double longest_side = max(vecRectsIn[ii].size.width, vecRectsIn[ii].size.height);
                double length_diff = longest_side / stripeheight_now;
                if (dist <= max_px_dist && (length_diff > stripediff_min && length_diff < stripediff_max )) {
                    incrnums[ii] = -1;//OK; Include it in the index
                    vecRectsCandidate.push_back(vecRectsIn[ii]);
                    vecRectsCandidateIdx.push_back(ii);

                    inliers_now++;
                    if (debugransac) {
                        circle(img_debug, vecRectsIn[ii].center, 10, Scalar(100,255,100), 3, CV_AA, 0);
                    }
                }
            } else {
                continue; //Continue if point already included
            }

        }

        if (inliers_now >= min_inliers) { //If we have found a succesful line
            //cout << "RANSAC found a line.." << endl;

            for (int ii = 0; ii < numpts; ii++) { //For all the points
                if (incrnums[ii] == -1) { //If this number is included in our line now
                    incrnumsOK[ii] = -1; //Exclude the points from the total array so it cant be chosen next time
                }
            }

            fitLine(cv::Mat( util::vecrotrect2vecpt(vecRectsCandidate)), line, CV_DIST_L2, 0, 0.01, 0.01);

            vecLines.push_back(line);
            //vector<Point> vecPtsNow;
            vecVecInlierIdx.push_back(vecRectsCandidateIdx);

            //circle(img_debug, Point(0,y), img_debug.rows/60, Scalar(100,200,100), 2,CV_AA,0);
            if (debugransac) {
                double m = 10000;
                //cv::line(img_debug, Point(line[2]-m*line[0], line[3]-m*line[1]), Point(line[2]+m*line[0], line[3]+m*line[1]), Scalar(0,0,255), 6, CV_AA, 0 );
                cv::line(img_debug, pt1, pt2, Scalar(0, 0, 255), 15, CV_AA, 0);
                //namedWindow("win", WINDOW_NORMAL);
                imwrite("/Users/tzaman/Desktop/bc/img_debug.tif", img_debug); 
                //exit(-1);
                //Mat img_tmp;
                //resize(img_debug, img_tmp, Size(), 0.05, 0.05);
                //imshow( "win", img_tmp );
                //waitKey(5000);
                //break;
            }
        }

        //cout << endl;
    }
}

} //END NAMESPACE openbarcode
