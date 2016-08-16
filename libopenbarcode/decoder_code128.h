/**
--------------------------------------------------------------------------------
-   Module      :   decoder_code128.h
-   Description :   1D and 2D C++ Barcode Library
-   Author      :   Tim Zaman, 11-AUG-2016
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
 --------------------------------------------------------------------------------
*/

#ifndef LIBOPENBARCODE_DECODER_CODE128_H_
#define LIBOPENBARCODE_DECODER_CODE128_H_

#include <string>
#include <vector>
#include <map>

#include "libopenbarcode/options.h"
#include "libopenbarcode/decoder_barcode.h"

namespace openbarcode {

class DecoderCode128 final : public DecoderBarcode {
 public:
    DecoderCode128();
    explicit DecoderCode128(Options *);
    virtual ~DecoderCode128();
  protected:
    int DecodeBinary(cv::Mat, openbarcode::code *) final;
};

} // namespace openbarcode

#endif //LIBOPENBARCODE_DECODER_CODE128_H_
