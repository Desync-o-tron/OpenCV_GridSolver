#include "stdafx.h"
#include "SVMsrc.h"
#include "Fire_Aux_Functions.h"
//#include "Handwriting_Grid_Aux_Functions.h"
#include "HandWritingMain.hpp"
#include "FireGridMain.hpp"
#include "LED_Functions.h"

using namespace cv;
using namespace std;

using std::cout; //whack
using cv::rectangle;

bool FIRE_GRID = false;
//bool END_INPUT_LOOP = false;

//VideoCapture cap(1); // open the camera
VideoCapture cap("BW_FG_DISTANCE_VARIATION.mkv");
//VideoCapture cap("BW_BOTH_DISTANCE_VARIATION_crop.mkv");
Mat frame;

int main()
{
    if (!cap.isOpened())
    {
        cout << "no cam found\n";
        return 1;
    }

    //vector<Mat> testImgs(9);
    //loadComparisonImages(testImgs, true);
    vector<Mat> nums;

    Mat num1 = imread("RealFireNumbers/1_REAL_CROP.png");
    Mat num2 = imread("RealFireNumbers/2_REAL_CROP.png");
    Mat num3 = imread("RealFireNumbers/3_REAL_CROP.png");
    Mat num4 = imread("RealFireNumbers/4_REAL_CROP.png");
    Mat num5 = imread("RealFireNumbers/5_REAL_CROP.png");
    Mat num6 = imread("RealFireNumbers/6_REAL_CROP.png");
    Mat num7 = imread("RealFireNumbers/7_REAL_CROP.png");
    Mat num8 = imread("RealFireNumbers/8_REAL_CROP.png");
    Mat num9 = imread("RealFireNumbers/9_REAL_CROP.png");

    nums.push_back(num1);
    nums.push_back(num2);
    nums.push_back(num3);
    nums.push_back(num4);
    nums.push_back(num5);
    nums.push_back(num6);
    nums.push_back(num7);
    nums.push_back(num8);
    nums.push_back(num9);


    //Ptr<SVM> model;
    //numPredictorSetup(model);
    //cout << endl;
    
    bool readyToFire = false;
    vector <int> returnGrid(9);

    Mat frame;
    while(1)
    {
        cap >> frame;
   
        FireGridMain(frame, nums, returnGrid, readyToFire);
        //HandWritingMain(frame, model, returnGrid, readyToFire);

        if (frame.empty())
            return 0;

        imshow("edges", frame);
        if (waitKey(10) >= 0) break;
        
    }

    return 0;
}
