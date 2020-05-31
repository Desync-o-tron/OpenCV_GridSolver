#pragma once
#include "stdafx.h"
#include "Generic_Grid_Functions.h"

bool massageImg(Mat& inFrame, Mat& outFrame);
bool contours2boundingRects(vector <vector<Point>>& contours, Mat frame, vector <RectStats>& rects);
bool filterRectsByAdjacency(Mat & frame, vector <RectStats> & rects);
bool deleteRectanglesNearScreenEdge(Mat& frame, vector <RectStats>& rects);
bool deleteSmallestTargets(vector<RectStats>& rects);
bool orderRectsByPos(vector<RectStats>& rects);
bool guessNumbers(Mat& frame, vector <RectStats>& rects, vector<Mat>& testImgs);
bool getTargetLock(vector<RectStats>& rects, vector<int>& lastValidGuessedNums, vector<int>& returnGrid, int& numberOfValidConsecutiveFrames); //returns true if readyToFire

