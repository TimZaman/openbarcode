# OpenBarcode C++ Library (libopenbarcode)
For 1D (barcode) and 2D (areacode) decoding and encoding.

# -> ! UNDER DEVELOPMENT ! <-

This repository contains the OpenBarcode source files for libopenbarcode. It solely depends on [opencv](https://github.com/itseez/opencv).

Written in C++, this library is intended to be cross-platform; Mac, Windows, iOS, Android, etc.

## Design Principles

### The Detector

For all 1D barcodes, the detectors works on bar-detection and then bar-clustering. All the 2D areacodes have their own distinct detector as these distinguish themselves generally by having their own markers. Each detected candidate will be converted into a bit or byte array that is fed into the decoder.

For an open sorce barcode library, this one distinguishes itself by the assumption that you are actually scanning a barcode 'straight', that means we do not assume large perspective or exotic spherical image transformations. We assume your camera is fairly parallel to the barcode surface, while it can be anywhere in the image and it can be rotated. In short, the detector is *rotation* invariant, *scale* invariant and *rotation* invariant. And it can handle only very _little_ shear, perspective and spherical distortion. 
This should be good for almost all use cases, except when you are trying to capture a barcode with a fisheye lens that's pasted on the outside of a pingpong ball. Although in the latter case the two distortions might cancel eachother out.

Assumptions:
- Fully scale, translation and rotation invariant.
- Only minor shear, perspective and spherical distortion allowed.
- Print quality has to be just fair, minor print defects and 'sparse ink' allowed.
- Print geometry should be correct. No skipped print-lines.
- Print background/foreground constrast needs to be good.
- Print should have at least 10% of its own size as edge-clearance.
- Multiple prints per image should have extra print-to-print clearance.
- Print should be free of any external overlap, small dust and crap is allowed.


### The Decoder

The decoder takes an array of bits and bytes as detected by a detector, and tries to decodes the data into the humanly readable format, with proper validation and error correction (if applicable).

### The Encoder

@TODO

## Features

- 100% written in modern C++11.
- Fast and reliable detection.
- Barcode can be anywhere in the image, works great even for small barcodes in very large images.
- The code should be in the image without perspective or spherical transformations. The detection is rotation invariant. This assumption makes detection very fast and robust in almost all cases, except when your camera ir viewing angle is exceptionally shitty.
- Supports most types of barcodes and areacodes.

## Supported Barcodes

TABLE HERE @TODO

1D:
Code39
?Code128

2D: 
Datamatrix
QR Code



## How To Use





##License
MIT
