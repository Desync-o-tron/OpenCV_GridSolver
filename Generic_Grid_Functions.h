#pragma once
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#include <opencv2/ml.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include <thread>
//#include <conio.h>
#include "opencv2/features2d/features2d.hpp"
#include <stack>

using namespace std;
using namespace cv;

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

class gridNum
{
public:
    int guessedNum;
    int gridPos; //1-9
    bool duplicate;
    vector <float> confidence; //from 0 to 1 i think, 0 to 8 corresp. w/ grid number
};

void gridErrorChecking2(vector <RectStats> & gridNums);
void gridErrorChecking(vector <gridNum> & gridNums, bool confidence);

bool display_Img(Mat frame);