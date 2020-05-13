#pragma once
#include "SVMsrc.h"
#include "Handwriting_Grid_Aux_Functions.h"
#include "Generic_Grid_Functions.h"

using namespace cv;
using namespace std;

//same as fire RectStats but with a img attched
struct HWRectStats 
{
    Rect roi;
    Mat img;
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


struct grid_pos {     
    Point pos; //upper lhc
    int posInOGvector;
};


void HandWritingMain(Mat & frame, Ptr<SVM> & model, vector <int> & returnGrid, bool & readyToFire);