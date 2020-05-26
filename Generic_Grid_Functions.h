#pragma once
#include "stdafx.h"

struct RectStats
{
    Rect roi;
    int vertConfs;
    int horizConfs;

    int gridPos; //1-9 top left to btm right
    vector<float> confidences;
    int guessedNum;
    bool duplicate;

    int X() { return roi.x; }
    int Y() { return roi.y; }
    float Area() { return (float)(roi.height*roi.width); }
    bool Valid() {
        if (horizConfs > 0 && vertConfs > 0 && horizConfs < 4)
            return true;
        else
            return false;
    }
};

struct gridNum
{
    int guessedNum;
    int gridPos; //1-9
    bool duplicate;
    vector <float> confidence; //from 0 to 1 i think, 0 to 8 corresp. w/ grid number
};

void gridErrorChecking2(vector <RectStats> & gridNums); //TODO get rid of one
void gridErrorChecking(vector <gridNum> & gridNums, bool confidence);

bool display_Img(Mat frame);