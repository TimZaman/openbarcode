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

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "gtest/gtest.h"

#include "libopenbarcode/options.h"  
#include "libopenbarcode/detector.h" 
#include "libopenbarcode/detector_barcode.h" 
#include "libopenbarcode/decoder.h" 
#include "libopenbarcode/decoder_code39.h" 

static const std::string image_directory = "../../sample_images/";

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

TEST (Code39Test, SampleImages) {
    cv::Mat im = cv::imread(image_directory + "c39.png");
    std::vector< openbarcode::Decoder * > decoders;
    openbarcode::Options opts;
    decoders.push_back(new openbarcode::DecoderCode39(&opts));
    openbarcode::DetectorBarcode dt_bc(&opts, decoders);
    dt_bc.setImage(im);
    dt_bc.Detect();
    dt_bc.Decode();

    std::vector< std::string > found_codes = dt_bc.getCodeStrings();

    ASSERT_EQ(1, found_codes.size());

    ASSERT_EQ("BM001190117", found_codes[0]);

    // Clean-up
    for (int i = 0; i < decoders.size(); i++) delete decoders[i];
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
