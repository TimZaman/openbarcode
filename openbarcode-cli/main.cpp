/**
--------------------------------------------------------------------------------
-   Module      :   main.cpp
-   Description :   Example of cli usage of 1D and 2D C++ Barcode Library
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

#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>

#include <opencv2/opencv.hpp>

#include "libopenbarcode/openbarcode_version.h"
#include "libopenbarcode/options.h"

#include "libopenbarcode/detector.h"
#include "libopenbarcode/detector_barcode.h"
#include "libopenbarcode/detector_dmtx.h"

#include "libopenbarcode/decoder.h"
#include "libopenbarcode/decoder_code39.h"
#include "libopenbarcode/decoder_dmtx.h"

using namespace std;

std::vector< std::string > dirToFilesVec(std::string path) {
    std::vector< std::string > files_vec;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        // print all the files and directories within directory
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                files_vec.push_back(std::string(ent->d_name));
            }
        }
        closedir (dir);
    } else {
        std::cerr << "Failed opening directory : " << path << std::endl;
        exit(-1);
    }
    return files_vec;
}

int main(int argc, char* argv[] ) {
    std::cout << "main() : this program provides a libopenbarcode example." << std::endl;
    std::cout << "OPENBARCODE_VERSION   = " << OPENBARCODE_VERSION << std::endl;
    std::cout << "OPENBARCODE_BUILDDATE = " << OPENBARCODE_BUILDDATE << std::endl;

    // Create the options
    openbarcode::Options opts;
    //opts.set

    opts.setValue("int", 1337);
    opts.setValue("string", "test");

    //cout << opts.getValue("int") << " ; " << opts.getValue("string") << endl;
    cout << opts.getValue<int>("int") << endl;
    int i = opts.getValue<int>("int");

    cout << opts.getValue<int>("default-test", 1234) << endl;

    //string files_dir = "/Users/tzaman/Dropbox/code/openbarcode/sample_images/C39/";
    string files_dir = "/Users/tzaman/Dropbox/code/openbarcode/sample_images/DMTX/";
    vector<string> files = dirToFilesVec(files_dir);

    for (int i = 0; i < files.size(); i++) {
        if (files[i].size()!=8) {
            continue;
        }

        cout << files[i] << endl;

        // Load a sample image
        cv::Mat im = cv::imread(files_dir + files[i]);
        
        // Create the decoder(s)
        std::vector< openbarcode::Decoder * > decoders;
        decoders.push_back(new openbarcode::DecoderDmtx(&opts));
        openbarcode::DetectorDmtx dt(&opts, decoders);
        //decoders.push_back(new openbarcode::DecoderCode39(&opts));
        //openbarcode::DetectorBarcode dt(&opts, decoders);
        dt.setImage(im);
        dt.Detect();
        dt.Decode();


        // Rename
        std::vector< std::string > found_codes = dt.getCodeStrings();
        if (found_codes.size() < 1) {
            exit(-1);
        }
        int rc = std::rename((files_dir + files[i]).c_str(), (files_dir + found_codes[0] + ".jpg").c_str() ); 

        for (int d = 0; d < decoders.size(); d++) delete decoders[d];
    }

    std::cout << "END main()" << std::endl;
}
