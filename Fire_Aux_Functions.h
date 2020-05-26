#pragma once
#include "stdafx.h"
#include "Generic_Grid_Functions.h"

bool massageImg(Mat& inFrame, Mat& outFrame);
bool contours2boundingRects(vector <vector<Point>>& contours, float imgArea, vector <RectStats>& rects);