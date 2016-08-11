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
#include <vector>
#include <tuple>

#include <dirent.h>
#include <sys/stat.h>

#include <opencv2/opencv.hpp>
#include "gtest/gtest.h"

#include "libopenbarcode/options.h"  

#include "libopenbarcode/detector.h" 
#include "libopenbarcode/detector_barcode.h" 
#include "libopenbarcode/detector_dmtx.h" 

#include "libopenbarcode/decoder.h" 
#include "libopenbarcode/decoder_code39.h" 
#include "libopenbarcode/decoder_dmtx.h" 

namespace testing
{
 namespace internal
 {
  enum GTestColor {
      COLOR_DEFAULT,
      COLOR_RED,
      COLOR_GREEN,
      COLOR_YELLOW
  };

  extern void ColoredPrintf(GTestColor color, const char* fmt, ...);
 }
}
#define PRINTF(...)  do { testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] "); testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__); } while(0)

// C++ stream interface
class TestCout : public std::stringstream
{
public:
    ~TestCout()
    {
        PRINTF("%s",str().c_str());
    }
};

#define TEST_COUT  TestCout()

std::string sample_folder = "../sample_images/";

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

inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

std::vector< std::string > dirToFilesVec(std::string path) {
    std::vector< std::string > files_vec;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        // print all the files and directories within directory
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                files_vec.push_back( path + std::string(ent->d_name));
            }
        }
        closedir (dir);
    } else {
        std::cerr << "Failed opening directory : " << path << std::endl;
        exit(-1);
    }
    return files_vec;
}


std::vector< std::string > splitFilename(const std::string& str) {
    // @TODO(tzaman) below only does the job if the input actually has a path, file and ext.
    std::vector< std::string > path_parts(3); // splits in <[0],[1],[2]> = <path, stem, extension>
    size_t idx_slash = str.find_last_of("/");
    path_parts[0] = str.substr(0, idx_slash+1);
    
    std::string filename = str.substr(idx_slash+1, std::string::npos);
    size_t idx_period = filename.find_first_of(".");

    path_parts[1] = filename.substr(0, idx_period);
    path_parts[2] = filename.substr(idx_period, std::string::npos);

    return path_parts;
}

/*
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
*/

/*
TEST (DmtxTest, ImagesNormal) {
    std::string path = sample_folder + "/DMTX/normal/";
    std::vector< std::string > files_vec = dirToFilesVec(path);
    for (int fi = 0; fi < files_vec.size(); fi++) {
        std::cout << "\t" << files_vec[fi] << std::endl;
        std::string expected_data = splitFilename(files_vec[fi], 0);
        cv::Mat im = cv::imread(path + files_vec[fi]);
        std::vector< openbarcode::Decoder * > decoders;
        openbarcode::Options opts;
        decoders.push_back(new openbarcode::DecoderDmtx(&opts));
        openbarcode::DetectorDmtx dt_dmtx(&opts, decoders);
        dt_dmtx.setImage(im);
        dt_dmtx.Detect();
        dt_dmtx.Decode();

        std::vector< std::string > found_codes = dt_dmtx.getCodeStrings();

        // Save candidates 
        //cv::Mat mat_draw = dt_dmtx.drawAllCandidates();
        //cv::imwrite("/Users/tzaman/Desktop/bc/dmtx_" + std::to_string(fi) + ".png", mat_draw);

        ASSERT_EQ(1, found_codes.size());
        ASSERT_EQ(expected_data, found_codes[0]);

        // Clean-up
        for (int d = 0; d < decoders.size(); d++) delete decoders[d];
    }
}
*/

enum {
    TEST_DET_DATAMATRIX,
    TEST_DET_QR,
    TEST_DET_BARCODE
};
enum {
    TEST_DEC_DATAMATRIX,
    TEST_DEC_QR,
    TEST_DEC_CODE39,
    TEST_DEC_CODE128,
};

class CombinationsTest :
    public ::testing::TestWithParam<std::tuple<int, int, std::string> > {};

void detectAndDecodeImage(int det_type, int dec_type, std::string filename) {
    TEST_COUT << filename << std::endl;
    ASSERT_TRUE(file_exists(filename));
    std::vector< std::string> file_parts = splitFilename(filename);
    std::string expected_data = file_parts[1]; // [1] is the stem
    
    cv::Mat im = cv::imread(filename);
    ASSERT_TRUE(im.data != NULL) << filename;

    std::vector< openbarcode::Decoder * > decoders;
    openbarcode::Options opts;
    if (dec_type == TEST_DEC_DATAMATRIX) {
        decoders.push_back(new openbarcode::DecoderDmtx(&opts));    
    } else if (dec_type == TEST_DEC_CODE39) {
        decoders.push_back(new openbarcode::DecoderCode39(&opts));    
    } else {
        // @TODO(tzaman) throw err
    }
    
    openbarcode::Detector * detector;
    if (dec_type == TEST_DEC_DATAMATRIX) {
        detector = new openbarcode::DetectorDmtx(&opts, decoders);
    } else if (dec_type == TEST_DET_BARCODE) {
        detector = new openbarcode::DetectorBarcode(&opts, decoders);
    } else {
        // @TODO(tzaman) throw err
    }
    
    detector->setImage(im);
    detector->Detect();
    detector->Decode();
    std::vector< std::string > found_codes = detector->getCodeStrings();
    ASSERT_EQ(1, found_codes.size());
    ASSERT_EQ(expected_data, found_codes[0]);
    
    // Clean-up
    for (int d = 0; d < decoders.size(); d++) {
        delete decoders[d];
    }
    delete detector;
}


TEST(Code39, ImagesNormal) {
    std::vector< std::string > filenames = dirToFilesVec(sample_folder + "/C39/normal/");
    for (int i = 0 ; i < filenames.size(); i++ ) 
        detectAndDecodeImage(TEST_DET_BARCODE, TEST_DEC_CODE39, filenames[i]);
}

TEST(Code39, ImagesPerfect) {
    std::vector< std::string > filenames = dirToFilesVec(sample_folder + "/C39/perfect/");
    for (int i = 0 ; i < filenames.size(); i++ ) 
        detectAndDecodeImage(TEST_DET_BARCODE, TEST_DEC_CODE39, filenames[i]);
}

TEST(Code39, ImagesGaps) {
    std::vector< std::string > filenames = dirToFilesVec(sample_folder + "/C39/gaps/");
    for (int i = 0 ; i < filenames.size(); i++ ) 
        detectAndDecodeImage(TEST_DET_BARCODE, TEST_DEC_CODE39, filenames[i]);
}

TEST(Datamatrix, ImagesNormal) {
    std::vector< std::string > filenames = dirToFilesVec(sample_folder + "/DMTX/normal/");
    for (int i = 0 ; i < filenames.size(); i++ )
        detectAndDecodeImage(TEST_DET_DATAMATRIX, TEST_DEC_DATAMATRIX, filenames[i]);
}

TEST(Datamatrix, ImagesPerfect) {
    std::vector< std::string > filenames = dirToFilesVec(sample_folder + "/DMTX/perfect/");
    for (int i = 0 ; i < filenames.size(); i++ )
        detectAndDecodeImage(TEST_DET_DATAMATRIX, TEST_DEC_DATAMATRIX, filenames[i]);
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

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
