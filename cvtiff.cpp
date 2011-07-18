#include "cvtiff.h"

#include <iostream>
#include <fstream>
#include <tiffio.h>

using namespace std;
using namespace cv;

typedef unsigned short ushort;

static Mat fail(const std::string & msg)
{
    cerr << "Could not load TIFF: " << msg << endl;
    return Mat(0, 0, CV_16UC3);
}

Mat cvtiffLoad16(const std::string & fname)
{
    // open the file
    TIFF * tiff = TIFFOpen(fname.c_str(), "r");
    if (!tiff)
        return fail("could not read from " + fname);

    // get the dimensions
    int width, height;
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);

    // read the image
    Mat result(height, width, CV_16UC3);
    for (int y = 0; y < height; ++y)
        TIFFReadScanline(tiff, result.ptr<ushort>(y), y);

    // close the file
    TIFFClose(tiff);
    return result;
}

void cvtiffSave16(const std::string & fname, const Mat & img)
{
    // open the file for writing
    TIFF * tiff = TIFFOpen(fname.c_str(), "w");
    if (!tiff)
    {
        fail("could not write to " + fname);
        return;
    }

    // set TIFF fields
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, img.cols);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, img.rows);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 1);

    TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    TIFFSetField(tiff, TIFFTAG_XRESOLUTION, 72.0);
    TIFFSetField(tiff, TIFFTAG_YRESOLUTION, 72.0);
    TIFFSetField(tiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

    // write the image
    for (int y = 0; y < img.rows; ++y)
        TIFFWriteScanline(tiff, (void *) img.ptr<ushort>(y), y);

    // close the file
    TIFFClose(tiff);
}
