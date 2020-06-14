#include "stdafx.h"
#include "FireGridMain.hpp"

void FireGridMain(Mat & frame, const vector<Mat> & testImgs, vector<int> & returnGrid, bool & readyToFire)
{
    //if (frame.empty())
    //{
    //    cout << "\nFRAME EMPTY\n";
    //    return;
    //}
    //if (testImgs[0].empty())
    //{
    //    cout << "\nTEST IMG EMPTY\n";
    //    return;
    //}

    static int numValidFrames = 0;
    static const int minValidFrames = 3;

    static vector <int> lastValidGuessedNums(9);
    static int numberOfValidConsecutiveFrames = 0;
    
    Mat filteredFrame;
    vector <vector <Point>> contours;
    vector <RectStats> rects;
    
    //TODO check if contour apprx algo can be changed for speed if you want this to run on a pi
    massageImg(frame, filteredFrame);
    findContours(filteredFrame, contours, RETR_LIST, CHAIN_APPROX_TC89_L1);
    contours2boundingRects(contours, frame, rects);

    //------- get rid of bad rectangles that made it this far ----------
    filterRectsByAdjacency(frame, rects);
    deleteRectanglesNearScreenEdge(frame, rects);
    deleteSmallestTargets(rects);

    if (!smallestTargetIsValid(rects))
        return; 
    
    if (rects.size() == 9) 
        numValidFrames++;
    else {
        numValidFrames = 0;
        return;
    }

    orderRectsByPos(rects);

    if (numValidFrames >= minValidFrames)
    {
        guessNumbers(frame, rects, testImgs);
        //gridErrorChecking2(rects); //TODO improve me
    }
    // --------------- finish number recognition -----------------/>
    readyToFire = getTargetLock(rects, lastValidGuessedNums, returnGrid, numberOfValidConsecutiveFrames);

    traceTargetOverlay(frame, rects, numValidFrames, readyToFire, minValidFrames);
    namedWindow("after process");
    imshow("filteredframe", filteredFrame);//
    waitKey(30);
}
