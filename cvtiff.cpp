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
        return fail("could not open " + fname);

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
        memcpy(result.ptr<ushort>(y), line, lineSize);
    }

    // close the file
    TIFFClose(tiff);
    _TIFFfree(line);
    return result;
}

void cvtiffSave16(const std::string & fname, const Mat & img)
{
}
