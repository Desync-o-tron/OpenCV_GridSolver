#include "stdafx.h"
#include "SVMsrc.h"
#include "Fire_Aux_Functions.h"
//#include "Handwriting_Grid_Aux_Functions.h"
#include "HandWritingMain.hpp"
#include "FireGridMain.hpp"
#include "LED_Functions.h"

using namespace cv;
using namespace std;

//using std::cout; //whack
//using cv::rectangle;

//VideoCapture cap(1); // open the camera
VideoCapture cap("firenums.avi");
//VideoCapture cap("handwrittennums.avi");

int main()
{
    if (!cap.isOpened())
    {
        cout << "no cam found\n";
        return 1;
    }

    vector<Mat> nums;
    Mat frame;
    bool readyToFire = false;
    vector <int> returnGrid(9);

    //HANDWRITING GRID IS BROKEN CURRENTLY.
    if (_HANDWRITING) {
        vector<Mat> testImgs(9);
        loadComparisonImages(testImgs, true);
        Ptr<SVM> model;
        numPredictorSetup(model);
        cout << endl;
        while (1) {
            cap >> frame;
            HandWritingMain(frame, model, returnGrid, readyToFire);
            if (frame.empty())
                return 0;
            imshow("edges", frame);
            if (waitKey(10) >= 0) break;
        }
    }
    
    if (_FIRE) {
        nums.push_back(imread("RealFireNumbers/1_REAL_CROP.png"));
        nums.push_back(imread("RealFireNumbers/2_REAL_CROP.png"));
        nums.push_back(imread("RealFireNumbers/3_REAL_CROP.png"));
        nums.push_back(imread("RealFireNumbers/4_REAL_CROP.png"));
        nums.push_back(imread("RealFireNumbers/5_REAL_CROP.png"));
        nums.push_back(imread("RealFireNumbers/6_REAL_CROP.png"));
        nums.push_back(imread("RealFireNumbers/7_REAL_CROP.png"));
        nums.push_back(imread("RealFireNumbers/8_REAL_CROP.png"));
        nums.push_back(imread("RealFireNumbers/9_REAL_CROP.png"));

        while (1) {
            cap >> frame;
            FireGridMain(frame, nums, returnGrid, readyToFire);
            if (frame.empty())
                break;
            imshow("edges", frame);
            if (waitKey(10) >= 0) break;
        }
    }

    return 0;
}
