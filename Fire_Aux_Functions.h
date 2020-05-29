#pragma once
#include "stdafx.h"
#include "Generic_Grid_Functions.h"

bool massageImg(Mat& inFrame, Mat& outFrame);
bool contours2boundingRects(vector <vector<Point>>& contours, Mat frame, vector <RectStats>& rects);
bool filterRectsByAdjacency(Mat & frame, vector <RectStats> & rects);
bool deleteRectanglesNearScreenEdge(Mat& frame, vector <RectStats>& rects);