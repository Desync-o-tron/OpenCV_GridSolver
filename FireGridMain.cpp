#include "stdafx.h"
#include "FireGridMain.hpp"

void FireGridMain(Mat & frame, vector<Mat> & testImgs, vector<int> & returnGrid, bool & readyToFire)
{
    if (frame.empty())
    {
        cout << "\nFRAME EMPTY\n";
        return;
    }
    if (testImgs[0].empty())
    {
        cout << "\nTEST IMG EMPTY\n";
        return;
    }

    static int success = 0;
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
    if (deleteSmallestTargets(rects) == false) //you could delete by largest diff from mean as well.
        return; //too many bad targets
    // ---------------- end deleting bad rectangles ---------------------------/>


    orderRectsByPos(rects);

    if (rects.size() == 9) // hmmm..
    {
        success++;
    }
    else
    {
        success = 0;
    }


    if (success >= minValidFrames)
    {
        guessNumbers(frame, rects, testImgs);
        gridErrorChecking2(rects); //TODO improve me
    }
    // --------------- finish number recognition -----------------/>
    readyToFire = getTargetLock(rects, lastValidGuessedNums, returnGrid, numberOfValidConsecutiveFrames);

    // ----------------draw rects ------------
    for (int i = 0; i < rects.size(); i++)
    {
        rectangle(frame, rects[i].roi, Scalar(255,255,0), 2);

        if (success >= minValidFrames)
        {
            string guessedNum = to_string(rects[i].guessedNum);
            putText(frame, guessedNum, Point(rects[i].X() + rects[i].roi.width, rects[i].Y() + 35), FONT_HERSHEY_DUPLEX, 1.5, Scalar(230, 0, 255), 2);
        }
        if (readyToFire)
        {
            rectangle(frame, rects[i].roi, Scalar(55, 55, 255), 3);
            //if( rects[i].guessedNum == targetNum)
            //  putText(frame, "FIRE NOW", Point(rects[i].X() + rects[i].roi.width, rects[i].Y() + 35), FONT_HERSHEY_DUPLEX, 1.5, Scalar(230, 0, 255), 2);
        }

        /*
        string vertConfs = "" + to_string(rects[i].vertConfs);
        string horizConfs= "" + to_string(rects[i].horizConfs);
        putText(frame, vertConfs, Point(rects[i].X(), rects[i].Y() + 14), FONT_HERSHEY_PLAIN, 1, Scalar(10, 0, 255),2);
        putText(frame, horizConfs, Point(rects[i].X(), rects[i].Y() + 30), FONT_HERSHEY_PLAIN, 1, Scalar(10, 0, 255),2);
        */
    }

    namedWindow("after process");
    imshow("filteredframe", filteredFrame);//
    waitKey(30);
}
