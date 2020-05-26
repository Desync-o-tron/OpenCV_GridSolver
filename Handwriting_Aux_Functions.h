#pragma once
#include "stdafx.h"
#include "Generic_Grid_Functions.h"

void contourOffset(std::vector<cv::Point>& src,
    std::vector<cv::Point>& dst, cv::Point& offset);

void scaleContour(std::vector<cv::Point>& src,
    std::vector<cv::Point>& dst, float scale);

void scaleContours(std::vector<std::vector<cv::Point>>& src,
    std::vector<std::vector<cv::Point>>& dst, float scale);
