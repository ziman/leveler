#include "cvtiff.h"

#include <iostream>
#include <fstream>
#include <tiffio.h>

using namespace std;
using namespace cv;

typedef unsigned short ushort;

Mat cvtiffLoad16(const std::string & fname)
{
    // open the file
    TIFF * tiff = TIFFOpen(fname.c_str(), "r");
    if (!tiff)
        return Mat(0, 0, CV_16U);

    // get the dimensions
    unsigned width, height;
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);

    // allocate the memory
    ushort * raster = (ushort *) _TIFFmalloc(width * height * 4 * sizeof(ushort));
    if (!raster)
        return Mat(0, 0, CV_16U);

    int ret = TIFFReadRGBAImage(tiff, width, height, (uint32 *) raster, 0);
    if (!ret)
        return Mat(0, 0, CV_16U);

    // close the file
    TIFFClose(tiff);

    // return the matrix (the allocated memory will be managed by the struct returned)
    return Mat(height, width, CV_16U, raster, width * 4 * sizeof(ushort));
}

void cvtiffSave16(const std::string & fname, const Mat & img)
{
}
