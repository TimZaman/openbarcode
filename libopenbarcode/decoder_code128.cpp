/**
--------------------------------------------------------------------------------
-   Module      :   decoder_code128.cpp
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


// From Wikipedia and using mapping defition inspired by Barcodesoft.com
static const char NUL     = 1;
static const char SOH     = 2;
static const char STX     = 3;
static const char ETX     = 4;
static const char EOT     = 5;
static const char ENQ     = 6;
static const char ACK     = 7;
static const char BEL     = 8;
static const char BS      = 9;
static const char TAB     = 10;
static const char LF      = 11;
static const char VT      = 12;
static const char FF      = 13;
static const char CR      = 14;
static const char SO      = 15;
static const char SI      = 16;
static const char DLE     = 17;
static const char DC1     = 18;
static const char DC2     = 19;
static const char DC3     = 20;
static const char DC4     = 21;
static const char NAK     = 22;
static const char SYN     = 23;
static const char ETB     = 24;
static const char CAN     = 25;
static const char EM      = 26;
static const char SUB     = 27;
static const char ESC     = 28;
static const char FS      = 29;
static const char GS      = 30;
static const char RS      = 31;
static const char US      = 32;
static const char DEL     = 127;
static const char CODE_A  = '0xF6';
static const char CODE_B  = '0xF5';
static const char CODE_C  = '0xF4';
static const char START_A = '0xF8';
static const char START_B = '0xF9';
static const char START_C = '0xFA';
static const char STOP    = '0xFB';
static const char FNC1    = '0xF7';
static const char FNC2    = '0xF2';
static const char FNC3    = '0xF1';
static const char FNC4    = '0xF3';
static const char SHIFT   = '0xF0';

static const int C128_NCHARACTERS = 114;
static const int C128_BARS_PER_CHAR = 10; // excluding end space 0

static const char* C128_Strings[C128_NCHARACTERS] = {
        "11011001100", "11001101100", "11001100110", "10010011000", "10010001100",
        "10001001100", "10011001000", "10011000100", "10001100100", "11001001000",
        "11001000100", "11000100100", "10110011100", "10011011100", "10011001110", 
        "10111001100", "10011101100", "10011100110", "11001110010", "11001011100", 
        "11001001110", "11011100100", "11001110100", "11101101110", "11101001100", 
        "11100101100", "11100100110", "11101100100", "11100110100", "11100110010", 
        "11011011000", "11011000110", "11000110110", "10100011000", "10001011000", 
        "10001000110", "10110001000", "10001101000", "10001100010", "11010001000", 
        "11000101000", "11000100010", "10110111000", "10110001110", "10001101110", 
        "10111011000", "10111000110", "10001110110", "11101110110", "11010001110", 
        "11000101110", "11011101000", "11011100010", "11011101110", "11101011000", 
        "11101000110", "11100010110", "11101101000", "11101100010", "11100011010", 
        "11101111010", "11001000010", "11110001010", "10100110000", "10100001100", 
        "10010110000", "10010000110", "10000101100", "10000100110", "10110010000", 
        "10110000100", "10011010000", "10011000010", "10000110100", "10000110010", 
        "11000010010", "11001010000", "11110111010", "11000010100", "10001111010", 
        "10100111100", "10010111100", "10010011110", "10111100100", "10011110100", 
        "10011110010", "11110100100", "11110010100", "11110010010", "11011011110", 
        "11011110110", "11110110110", "10101111000", "10100011110", "10001011110", 
        "10111101000", "10111100010", "11110101000", "11110100010", "10111011110", 
        "10111101110", "11101011110", "11110101110", "11010000100", "11010010000", 
        "11010011100", "11000111010", "11010000100", "11010010000", "11010011100", 
        "11101011110", "10111101110", "10111011110", "11000111010"
    };

static const char C128A_Characters[C128_NCHARACTERS] = {
        ' ', '!', '\"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-',
        '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';',
        '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
        'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
        'X', 'Y', 'Z', '[', '\\', ']', '^', '_', NUL, SOH, STX, ETX, EOT, ENQ,
        ACK, BEL, BS, TAB, LF, VT, FF, CR, SO, SI, DLE, DC1, DC2, DC3, DC4,
        NAK, SYN, ETB, CAN, EM, SUB, ESC, FS, GS, RS, US, FNC3, FNC2, SHIFT,
        CODE_C, CODE_B, FNC4, FNC1, START_A, START_B, START_C, STOP, START_A,
        START_B, START_C, FNC4, CODE_B, CODE_C, STOP
   };

static const char C128B_Characters[C128_NCHARACTERS] = {
        ' ', '!', '\"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', 
        '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', 
        '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 
        'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 
        'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 
        'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 
        't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', DEL, FNC3, FNC2, 
        SHIFT, CODE_C, FNC4, CODE_A, NC1, START_A, START_B,START_C,STOP, 
        START_A,START_B,START_C,CODE_A, FNC4, CODE_C, STOP
   };

// Note the numbers for C128 are converted in to chars, but should be 2-digit numbers.
static const char C128C_Characters[C128_NCHARACTERS] = {
        00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
        60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
        72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
        84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
        96, 97, 98, 99, CODE_B, CODE_A, FNC1, START_A, START_B, START_C,
        STOP, START_A, START_B, START_C, CODE_A, CODE_B, 99, STOP
   };

static const char C128_Characters[3] = {C128A_Characters, C128B_Characters, C128C_Characters};

static const std::map< std::string, char > generateDecodingMap() {
    std::map<std::string, char> mapping;
    for (int i = 0; i < C128_NCHARACTERS; i++) {
        mapping[C128_Strings[i]] = C128_Characters[i];
    }
    return mapping;
}

static const std::map<std::string, char> DECODINGMAP_C128 = generateDecodingMap();


// @TODO(tzaman)