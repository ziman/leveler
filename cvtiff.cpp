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

    // allocate the memory
    int lineSize = TIFFScanlineSize(tiff);
    ushort * line = (ushort *) _TIFFmalloc(lineSize);
    if (!line)
        return fail("could not allocate scanline");

    // cout << "scanline size: " << lineSize << ", width: " << width << ", bytes per pixel: " << lineSize/width << endl;

    // read the image
    Mat result(height, width, CV_16UC3);
    for (int y = 0; y < height; ++y)
    {
        TIFFReadScanline(tiff, line, y);
        ushort * src = line;
        ushort * row = result.ptr<ushort>(y);

        // convert RGB -> BGR
        for (int x = 0; x < width; ++x)
        {
            row[0] = src[2];
            row[1] = src[1];
            row[2] = src[0];

            row += 3;
            src += 3;
        }
    }

    // close the file
    TIFFClose(tiff);
    _TIFFfree(line);
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

    // allocate the memory
    int lineSize = TIFFScanlineSize(tiff);
    ushort * line = (ushort *) _TIFFmalloc(lineSize);
    if (!line)
    {
        fail("could not allocate scanline");
        TIFFClose(tiff);
        return;
    }

    //cout << "scanline size: " << lineSize << ", width: " << img.cols << ", bytes per pixel: " << lineSize/img.cols << endl;

    // write the image
    for (int y = 0; y < img.rows; ++y)
    {
        ushort * dst = line;
        const ushort * row = img.ptr<ushort>(y);

        // convert RGB -> BGR
        for (int x = 0; x < img.cols; ++x)
        {
            dst[0] = row[2];
            dst[1] = row[1];
            dst[2] = row[0];

            row += 3;
            dst += 3;
        }
        TIFFWriteScanline(tiff, line, y);
    }

    // close the file
    TIFFClose(tiff);
    _TIFFfree(line);
}
