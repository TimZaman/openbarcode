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

static const std::map<std::string, char> decoding = generateDecodingMap();

namespace openbarcode {


DecoderCode39::DecoderCode39(Options * opts) : Decoder(opts) {
    std::cout << "DecoderCode39::DecoderCode39()" << std::endl;
}

DecoderCode39::~DecoderCode39() {
    std::cout << "DecoderCode39::~DecoderCode39()" << std::endl;

}





} //END NAMESPACE openbarcode
