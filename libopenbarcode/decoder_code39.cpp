/**
--------------------------------------------------------------------------------
-   Module      :   decoder_code39.cpp
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 5-AUG-2016
--------------------------------------------------------------------------------
*/

#include <iostream>
#include <string>
#include <vector>

#include "libopenbarcode/toolkit/utils_opencv.h"
#include "libopenbarcode/toolkit/utils_general.h"

#include "libopenbarcode/decoder_code39.h"

static const char C39_SENTINEL = '*';
static const int C39_CHARACTERS = 44;
static const char C39_SENTINEL_STRING[] = "nwnnwnwnn";

static const char C39_Characters[C39_CHARACTERS] = {'0','1','2','3','4','5','6','7',
                                                    '8','9','A','B','C','D','E','F',
                                                    'G','H','I','J','K','L','M','N',
                                                    'O','P','Q','R','S','T','U','V',
                                                    'W','X','Y','Z','-','.',' ','$',
                                                    '/','+','%', C39_SENTINEL};

static const char* C39_Strings[C39_CHARACTERS] = {"nnnwwnwnn", "wnnwnnnnw", "nnwwnnnnw",
                                                  "wnwwnnnnn", "nnnwwnnnw", "wnnwwnnnn",
                                                  "nnwwwnnnn", "nnnwnnwnw", "wnnwnnwnn",
                                                  "nnwwnnwnn", "wnnnnwnnw", "nnwnnwnnw",
                                                  "wnwnnwnnn", "nnnnwwnnw", "wnnnwwnnn",
                                                  "nnwnwwnnn", "nnnnnwwnw", "wnnnnwwnn",
                                                  "nnwnnwwnn", "nnnnwwwnn", "wnnnnnnww",
                                                  "nnwnnnnww", "wnwnnnnwn", "nnnnwnnww",
                                                  "wnnnwnnwn", "nnwnwnnwn", "nnnnnnwww",
                                                  "wnnnnnwwn", "nnwnnnwwn", "nnnnwnwwn",
                                                  "wwnnnnnnw", "nwwnnnnnw", "wwwnnnnnn",
                                                  "nwnnwnnnw", "wwnnwnnnn", "nwwnwnnnn",
                                                  "nwnnnnwnw", "wwnnnnwnn", "nwwnnnwnn",
                                                  "nwnwnwnnn", "nwnwnnnwn", "nwnnnwnwn",
                                                  "nnnwnwnwn", C39_SENTINEL_STRING};

static const std::map< std::string, char > generateDecodingMap() {
    std::map<std::string, char> mapping;
    for(int i = 0; i < C39_CHARACTERS; i++) {
        mapping[C39_Strings[i]] = C39_Characters[i];
    }
    return mapping;
}

static const std::map<std::string, char> DECODINGMAP_C39 = generateDecodingMap();

namespace openbarcode {


DecoderCode39::DecoderCode39(Options * opts) : Decoder(opts) {
    std::cout << "DecoderCode39::DecoderCode39()" << std::endl;
}

DecoderCode39::~DecoderCode39() {
    std::cout << "DecoderCode39::~DecoderCode39()" << std::endl;

}


int DecoderCode39::DecodeBinary(cv::Mat image, openbarcode::code * current_code) {
    //stripeCode codeNow;
    //Walk the code and extract words

    std::vector<double> intervals_black;
    std::vector<double> intervals_white;
    //We will now perform smart measurement of the white and black stripes.
    char prev_char=image.at<char>(0);
    int current_length=0;
    for (int i=1; i<image.rows * image.cols; i++){
        char current_char=image.at<char>(i);
        current_length++;
        if (current_char!=prev_char){
            //Switch
            if (prev_char==0){
                intervals_black.push_back(current_length);
            } else {
                intervals_white.push_back(current_length);
            }
            current_length=0; //reset
        }
        prev_char = current_char;
    }

    intervals_white.erase(intervals_white.begin()); //erase first white index (as the barcode itself starts with black, but background with white)

    int width_wide_narrow_division_white = util::calcMeanOfQuarterAndThreeQuarterPercentile(intervals_white);
    int width_wide_narrow_division_black = util::calcMeanOfQuarterAndThreeQuarterPercentile(intervals_black);
    //Account for the fact that character spacing is thin and more thin divisors that wide
    width_wide_narrow_division_white *= 1.3;

    //Account for the fact that at most 2/5 are black wide.
    width_wide_narrow_division_black *= 1.1;

    double width_max = std::max(width_wide_narrow_division_white, width_wide_narrow_division_black) * 2.5;

    /*
    cout << "intervals_white=" << endl;
    for (int i=0; i<intervals_white.size(); i++){
        cout << intervals_white[i] << ",";
    }
    cout << endl;
    cout << "intervals_black=" << endl;
    for (int i=0; i<intervals_black.size(); i++){
        cout << intervals_black[i] << ",";
    }
    cout << endl;
    cout << "width_wide_narrow_division_white=" << width_wide_narrow_division_white << endl;
    cout << "width_wide_narrow_division_black=" << width_wide_narrow_division_black << endl;
    */


    std::vector<char> words;
    char prevbc=255; //white is start and base color
    int lengthnow=0;
    for (int i=0; i<image.rows * image.cols; i++){
        char curbc = image.at<char>(i); //Current value (1/0)
        if (curbc!=prevbc){ //1/0 switch
            //if ((lengthnow > width_min) && (lengthnow < width_max)) {
            if ((lengthnow < width_max)) {
                int division_width_threshold;
                if (prevbc==0){
                    division_width_threshold = width_wide_narrow_division_black;
                } else {
                    division_width_threshold = width_wide_narrow_division_white;
                }
                words.push_back(lengthnow > division_width_threshold ? 'w' : 'n');
                
            } else {
                //cout << "word ignored. l=" << lengthnow << endl;
                words.push_back('X');
            }
            //cout << lengthnow << " ";
            lengthnow=0; //Reset length
        }
        prevbc = curbc;
        lengthnow++;
    } 
    //cout << endl;

    std::string bc_string(words.begin(), words.end());

    //We now have a word array in which 'n'=narrow, 'w'=wide, 'X'=very wide/invalid

    /*
    cout << "Words(" << words.size() <<"):" << endl;
    for (int j=0; j<words.size(); j++){
        cout << words[j];
    }
    cout << endl;
    */


    //Generate the decoding map
    //std::map<std::string, char> decoding = generateDecodingMap();

    //Look for start and stop asterisk
    int bc_start_idx = -1;
    int bc_stop_idx = -1;
    int max_asterisk_start_bars = 9;//Maximum bars that can be passed before asterisk is found

    for (int rev = 0; rev <= 1; rev++) {
        for (int j = 0; j < max_asterisk_start_bars; j++) {
            std::map<std::string, char>::const_iterator it;
            std::string curr = bc_string.substr(j, 9);
            it = DECODINGMAP_C39.find(curr);
            if(it == DECODINGMAP_C39.end()){
                //cout << "NOPE." << endl;
            } else if (it->second == C39_SENTINEL) {
                bc_start_idx = j + 10;
                //cout << "FOUND:" << it->second << endl;
            }
        }

        
        for (int j = bc_string.length() - 9; j > bc_string.length() - max_asterisk_start_bars - 9; j--) {
            std::map<std::string, char>::const_iterator it;
            std::string curr = bc_string.substr(j, 9);
            it = DECODINGMAP_C39.find(curr);
            if (it == DECODINGMAP_C39.end()) {
                //cout << "NOPE." << endl;
            } else if (it->second == C39_SENTINEL) {
                bc_stop_idx = j;
                //cout << "FOUND:" << it->second << endl;
            }
        }
        if (bc_start_idx != -1 && bc_stop_idx != -1) { 
            break; //Found!
        } else { //Not found!
            //Try in reverse, reverse the string
            
            std::reverse(std::begin(bc_string), std::end(bc_string));
        }
    }


    if (bc_start_idx == -1) {
        std::string strErr = "Code 39 start asterisk not found.";
        std::cout << strErr << std::endl;
        return RET_NONE_FOUND;
    } else if (bc_stop_idx == -1) {
        std::string strErr = "Code 39 stop asterisk not found.";
        std::cout << strErr << std::endl;
        return RET_NONE_FOUND;
    }

    std::cout << "bc_start_idx=" << bc_start_idx << " bc_stop_idx=" << bc_stop_idx << std::endl;

    std::string strRaw = bc_string.substr(bc_start_idx, bc_stop_idx-bc_start_idx + 9);

    /*if (((bc_stop_idx-bc_start_idx)%10)!=0){
        string strErr = "Code39 bar count not a modulo of 10, len=" + std::to_string(bc_stop_idx-bc_start_idx);
        cout << strErr << endl;
        return vecStripecodes;
    }*/
    
    //Decode the bitch
    std::string barcode;
    bool decodingAllOk = true;
    for (int j = bc_start_idx; j < bc_stop_idx; j += 10) {
        std::string curr = bc_string.substr(j, 9);
        //cout << j << " curr=" << curr << endl;

        std::map<std::string, char>::const_iterator it;
        it = DECODINGMAP_C39.find(curr);
        if(it == DECODINGMAP_C39.end()){
            std::string strErr = "Code 39 unknown decoding sequence: '" + curr + "' from position " + std::to_string(j); 
            std::cout << strErr << std::endl;
            //return vecStripecodes;
            decodingAllOk=false;
            break;
        } else {
            barcode +=(it->second);
        }
    }
    if (!decodingAllOk) {
        std::string strErr = "Code 39 found unknown decoding sequence somewhere, ignoring..";
        return RET_NONE_FOUND;
    }

    //cout << "BARCODE: " << barcode << " raw=" << strRaw << endl;

    current_code->data = barcode;
    current_code->data_raw = strRaw;

    return RET_SUCCESS;
}

int DecoderCode39::Decode(cv::Mat image, openbarcode::code * current_code) {
    std::cout << "DecoderCode39::Decode()" << std::endl;

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

// @TODO(tzaman): do the below in the detector.cpp ?
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
