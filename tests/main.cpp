/**
--------------------------------------------------------------------------------
-   Module      :   main.cpp
-   Description :   C++ Unit Testing of (lib)openbarcode
-   Author      :   Tim Zaman, 7-AUG-2016
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

#include <unistd.h>
#include <iostream>
#include <string>

#include <dirent.h>

#include <opencv2/opencv.hpp>
#include "gtest/gtest.h"

#include "libopenbarcode/options.h"  
#include "libopenbarcode/detector.h" 
#include "libopenbarcode/detector_barcode.h" 
#include "libopenbarcode/decoder.h" 
#include "libopenbarcode/decoder_code39.h" 

std::string sample_folder = "../sample_images/";

double squareroot (const double val) {
    return val / 2.0;
}

/*
TEST(test_case_name, test_name) {
    EXPECT_TRUE(condition)
    ASSERT_TRUE(condition);
    ASSERT_GT(val1,val2);
    ASSERT_EQ(val1,val2);
    ASSERT_NE(val1,val2);
    ASSERT_STREQ(str1,str2); // for char[]'s
    ASSERT_STRNE(str1,str2); // for char[]'s
    ASSERT_STRCASEEQ(str1,str2); // for char[]'s
}
*/

std::vector< std::string > dirToFilesVec(std::string path) {
    std::vector< std::string > files_vec;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            
            if (ent->d_name[0] != '.') {
                printf ("%s\n", ent->d_name);
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


std::string splitFilename(const std::string& str, int iopt) {
    // Splits a filename into a stem and extension; 0:stem 1:ext
    size_t found = str.find_last_of(".");
    if (iopt == 0) {
        return str.substr(0, found);
    } else {
        return str.substr(found + 1);
    }
}


TEST (Code39Test, SampleImages) {
    std::string path = sample_folder + "/C39/";
    std::vector< std::string > files_vec = dirToFilesVec(path);
    for (int fi = 0; fi < files_vec.size(); fi++) {
        std::string expected_data = splitFilename(files_vec[fi], 0);
        cv::Mat im = cv::imread(path + files_vec[fi]);
        std::vector< openbarcode::Decoder * > decoders;
        openbarcode::Options opts;
        decoders.push_back(new openbarcode::DecoderCode39(&opts));
        openbarcode::DetectorBarcode dt_bc(&opts, decoders);
        dt_bc.setImage(im);
        dt_bc.Detect();
        dt_bc.Decode();

        std::vector< std::string > found_codes = dt_bc.getCodeStrings();

        ASSERT_EQ(1, found_codes.size());
        ASSERT_EQ(expected_data, found_codes[0]);

        // Clean-up
        for (int d = 0; d < decoders.size(); d++) delete decoders[d];
    }
}

void printHelp() {
    std::cout << "Usage:" << std::endl;
    std::cout << " -f  sets folder with sample images" << std::endl;
    // std::cout << " -h  prints help message" << std::endl;
    std::cout << " .. GoogleTest flags are also supported" << std::endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printHelp();
    }

    ::testing::InitGoogleTest(&argc, argv);
    for (int i = 1; i < argc; i++) {
        std::cout << argv[i] << std::endl;
        if (std::string(argv[i]).compare("-f") == 0) {
            if (argc >= i+1) {
                sample_folder = argv[i + 1];
                std::cout << "sample_folder set to : " << sample_folder << std::endl;  
            } else {
                std::cerr << "No sample folder argument supplied." << std::endl;
                exit(-1);
            }
        }
    }

    return RUN_ALL_TESTS();
}
