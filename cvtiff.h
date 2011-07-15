#ifndef CVTIFF_H
#define CVTIFF_H

#include <string>
#include <opencv.hpp>

cv::Mat cvtiffLoad16(const std::string & fname);
void cvtiffSave16(const std::string & fname, const cv::Mat & img);

#endif
