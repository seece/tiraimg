tiraimg
=======

![Compression demonstration](https://github.com/seece/tiraimg/blob/master/docs/muuri_pakkaus.png "")

## Introduction
*tiraimg* is a JPEG-clone. It features a compressor/decompressor for PPM image format.

## Building
Build prerequisites:

	libnetpbm10-dev
	gcc 4.6.3 or newer

To build the project:

	$ make all	

When the project has been built you can run the tests with

	$ make check

## Usage
To compress use

	$ tiraimg -q 50 source.ppm destination.timg

and for decompression type

	$ tiraimg -d destination.timg source.ppm

See ```tiraimg -h``` for more. 
