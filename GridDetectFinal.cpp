#include "stdafx.h"
#include "Fire_Aux_Functions.h"
#include "FireGridMain.hpp"

#include "ErrorChecking.h" //

//VideoCapture cap(1); // open the camera
VideoCapture cap("firenums.avi");

int main()
{
    vector < vector <int>> inputSets;
    vector < vector <int>> outputSets;
    vector<int> positions(vector<int>({ 0,0,0 }));
    /*inputSets.push_back(vector<int>({ 1,2,3 }));
    inputSets.push_back(vector<int>({ 4,5,6 }));
    inputSets.push_back(vector<int>({ 7,8,9 }));*/
    inputSets.push_back(vector<int>({ 0,4,6 }));
    inputSets.push_back(vector<int>({ 0,1,2 }));
    inputSets.push_back(vector<int>({ 0,1,2 }));
    
    //positions.push_back();
    //positions.assign
    outputSets = generateGridGuesses(inputSets, positions);
    cout << endl;

    for (auto set : outputSets) {
        for (int num : set) {
            cout << num << ", ";
        }
        cout << endl;
    }


    return 0;

    if (!cap.isOpened())
    {
        cout << "no cam found\n";
        return 1;
    }

    Mat frame;
    bool readyToFire = false;
    vector <int> returnGrid(9);
    vector<Mat> comparisonNums; // TODO: preshrink for speed?
    comparisonNums.push_back(imread("RealFireNumbers/1_REAL_CROP.png"));
    comparisonNums.push_back(imread("RealFireNumbers/2_REAL_CROP.png"));
    comparisonNums.push_back(imread("RealFireNumbers/3_REAL_CROP.png"));
    comparisonNums.push_back(imread("RealFireNumbers/4_REAL_CROP.png"));
    comparisonNums.push_back(imread("RealFireNumbers/5_REAL_CROP.png"));
    comparisonNums.push_back(imread("RealFireNumbers/6_REAL_CROP.png"));
    comparisonNums.push_back(imread("RealFireNumbers/7_REAL_CROP.png"));
    comparisonNums.push_back(imread("RealFireNumbers/8_REAL_CROP.png"));
    comparisonNums.push_back(imread("RealFireNumbers/9_REAL_CROP.png"));

    if (comparisonNums[0].empty())
    {
        cout << "COMPARISON IMG EMPTY\n";
        return 2;
    }

    while (1) {
        cap >> frame;
        if (frame.empty()) {
            cout << "\nFRAME EMPTY\n";
            break;
        }
        FireGridMain(frame, comparisonNums, returnGrid, readyToFire);
        imshow("edges", frame);
        if (waitKey(10) >= 0) break;
    }

    return 0;
}
