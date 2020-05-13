#include "stdafx.h"
#include "Fire_Aux_Functions.h"

void loadComparisonImages(vector <Mat> & nums, bool BGR2GRAY_BOOL )
{
    if (BGR2GRAY_BOOL)
    {

        Mat num1 = imread("RealFireNumbers/1_REAL_CROP.png");
        Mat num2 = imread("RealFireNumbers/2_REAL_CROP.png");
        Mat num3 = imread("RealFireNumbers/3_REAL_CROP.png");
        Mat num4 = imread("RealFireNumbers/4_REAL_CROP.png");
        Mat num5 = imread("RealFireNumbers/5_REAL_CROP.png");
        Mat num6 = imread("RealFireNumbers/6_REAL_CROP.png");
        Mat num7 = imread("RealFireNumbers/7_REAL_CROP.png");
        Mat num8 = imread("RealFireNumbers/8_REAL_CROP.png");
        Mat num9 = imread("RealFireNumbers/9_REAL_CROP.png");

        cvtColor(num1, num1, COLOR_BGR2GRAY);
        cvtColor(num2, num2, COLOR_BGR2GRAY);
        cvtColor(num3, num3, COLOR_BGR2GRAY);
        cvtColor(num4, num4, COLOR_BGR2GRAY);
        cvtColor(num5, num5, COLOR_BGR2GRAY);
        cvtColor(num6, num6, COLOR_BGR2GRAY);
        cvtColor(num7, num7, COLOR_BGR2GRAY);
        cvtColor(num8, num8, COLOR_BGR2GRAY);
        cvtColor(num9, num9, COLOR_BGR2GRAY);

        nums.push_back(num1);
        nums.push_back(num2);
        nums.push_back(num3);
        nums.push_back(num4);
        nums.push_back(num5);
        nums.push_back(num6);
        nums.push_back(num7);
        nums.push_back(num8);
        nums.push_back(num9);

        //imshow("asdf", nums[0]);
        //waitKey(0);
    }
    else
    {
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
    }
}
