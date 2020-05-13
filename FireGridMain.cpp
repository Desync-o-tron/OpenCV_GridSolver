#include "stdafx.h"
#include "FireGridMain.hpp"
void FireGridMain(Mat & frame, vector<Mat> & testImgs, vector<int> & returnGrid, bool & readyToFire)
{
    if (frame.empty() )
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
    static int numSuccessFrames = 3;

    static vector <int> lastValidGuessedNums(9);
    static int numberOfValidConsecutiveFrames = 0;
    //static bool FIRE_NOW = false;

    Mat frame2 = frame.clone();
    cvtColor(frame2, frame2, COLOR_BGR2GRAY);

    //blurring action
    Mat element = getStructuringElement(MORPH_RECT,Size(4, 4),Point(0, 0));
    dilate(frame2, frame2, element);
    dilate(frame2, frame2, element);
    element = getStructuringElement(MORPH_RECT,Size(7, 7),Point(0, 0));
    erode(frame2, frame2, element);

    threshold(frame2, frame2, 60, 255, THRESH_BINARY);

    vector <vector <Point>> contours;
    findContours(frame2, contours, RETR_LIST, CHAIN_APPROX_TC89_L1);
    //TODO check if contour apprx algo can be changed for speed

    Rect roi;
    vector <RectStats> rects;

    for (int i = 0; i < contours.size(); i++)
    {
        float area = (float)(contourArea(contours[i]));

        if (area > frame.cols*frame.rows*0.00054 && area < frame.cols*frame.rows*0.03)
        {
            RectStats RS;
            Rect roi = boundingRect(contours[i]);
            RS.roi = roi;

            if (roi.height > roi.width*0.62)
            {
               // if(roi.height >= 8 && roi.width >= 8 &&
               // roi.x > 0 && roi.x < frame.cols &&
               // roi.y > 0 && roi.y < frame.rows)

                rects.push_back(RS);

            }

        }
    } //end sorting by area and dimension ratio


    //----------start adjacency algo ------------------------------------------

    for (int i = 0; i < rects.size(); i++)
    {
        rects[i].horizConfs = 0;
        rects[i].vertConfs = 0;
    }

    float areaTolMult = 1.65; //1.50 too small
    float closeTolMult = 0.5;

    int horizTol_dstToNeigh = frame.cols*0.25;
    int vertTol_dstToNeigh = frame.rows*0.20;

    for (int i = 0; i < rects.size(); i++)
    {
        //to check if the rectangles are in the same col or row
        int horizTol_inCol = rects[i].roi.width*closeTolMult;
        int vertTol_inRow = rects[i].roi.height*closeTolMult;

        for (int j = 0; j < rects.size(); j++)
        {
            if (j != i)
            {
                Rect roiUp;
                roiUp.height = vertTol_dstToNeigh;
                roiUp.width = horizTol_inCol*2;
                roiUp.y = rects[i].roi.y - roiUp.height;
                roiUp.x = rects[i].roi.x - roiUp.width / 2.0;
                //rectangle(frame, roiUp, Scalar(200, 0, 200));

                Rect roiDown;
                roiDown.height = vertTol_dstToNeigh;
                roiDown.width = horizTol_inCol * 2;
                roiDown.y = rects[i].roi.y;// +roiDown.height;
                roiDown.x = rects[i].roi.x - roiDown.width / 2.0;
                //rectangle(frame, roiDown, Scalar(150, 0, 250));

                Rect roiLeft;
                roiLeft.height = vertTol_inRow * 2;
                roiLeft.width = horizTol_dstToNeigh;
                roiLeft.y = rects[i].roi.y - roiLeft.height / 2.0;
                roiLeft.x = rects[i].roi.x - roiLeft.width;
                //rectangle(frame, roiLeft, Scalar(150, 0, 250));

                Rect roiRight;
                roiRight.height = vertTol_inRow * 2;
                roiRight.width = horizTol_dstToNeigh;
                roiRight.y = rects[i].roi.y - roiRight.height / 2.0;
                roiRight.x = rects[i].roi.x;// roiRight.width;
                //rectangle(frame, roiRight, Scalar(150, 0, 250));

                if (rects[i].Area() / areaTolMult <= rects[j].Area() && rects[j].Area() <= rects[i].Area()*areaTolMult)
                {
                    if (roiUp.contains(Point(rects[j].X(), rects[j].Y())))
                        rects[i].vertConfs++;

                    if (roiDown.contains(Point(rects[j].X(), rects[j].Y()))) //else's?
                        rects[i].vertConfs++;

                    if (roiLeft.contains(Point(rects[j].X(), rects[j].Y())))
                        rects[i].horizConfs++;

                    if (roiRight.contains(Point(rects[j].X(), rects[j].Y())))
                        rects[i].horizConfs++;
                }
            }
        }
    }

    for (int i = 0; i < rects.size();)
    {
        if (!rects[i].Valid())
            rects.erase(rects.begin() + i);
        else
            i++;
    }
    //---------------------- end adjacnency algo ------------------------------------------


    //------- get rid of bad rectangles that made it this far ----------

    //delete rectangles close to screen edge
    for (int i = 0; i < rects.size();)
    {
        if (rects[i].X() < 15)
            rects.erase(rects.begin() + i);
        else if (rects[i].X() > frame.cols - rects[i].roi.width - 15)
            rects.erase(rects.begin() + i);
        else if (rects[i].Y() < 15)
            rects.erase(rects.begin() + i);
        else if (rects[i].Y() > frame.rows - rects[i].roi.height - 15)
            rects.erase(rects.begin() + i);
        else
            i++;
    }

    // TODO improve me
    while (rects.size() > 9)
    {
        //  delete smallest rectangles
        sort(rects.begin(), rects.end(),
            [](RectStats const& a, RectStats const& b) { return (a.roi.height*a.roi.width) < (b.roi.height*b.roi.width); });
        rects.erase(rects.begin() + 0);

    }
    // ---------------- end deleting bad rectangles ---------------------------/>

    // ----- organize grid pos 1 - 9 -----
    if (rects.size() == 9)
    {

        sort(rects.begin(), rects.end(),
            [](RectStats const& a, RectStats const& b) { return a.roi.y < b.roi.y; });

        vector <RectStats> organizedRects;

        for(int a = 0; a < 3; a++)
        {
            vector< RectStats> row;
            for (int i = 0; i < 3; i++)
            {
                row.push_back(rects[0]);
                rects.erase(rects.begin() + 0);
            }

            sort(row.begin(), row.end(),
                [](RectStats const& a, RectStats const& b) { return a.roi.x < b.roi.x; });

            for (int i = 0; i < 3; i++)
                organizedRects.push_back(row[i]);
        }

        rects = organizedRects;
        for (int i = 0; i < 9; i++)
            rects[i].gridPos = i + 1;
    }
    else
    {
        for (int p = 0; p < rects.size(); p++)
        {
          rects[p].gridPos = 0;
          rects[p].guessedNum = 0;
        }
    }
    //------- end grid organization -------------

    if (rects.size() == 9)
    {
        success++;
    }
    else
    {
        success = 0;
    }


    // --------------------------- match template ---------------------------
    if (success >= numSuccessFrames)
    {
        for (int x = 0; x < rects.size(); x++)
        {
            //TODO lower these to improve accuracy?
            Rect roi_larger = rects[x].roi;
            roi_larger.x -= 5;
            roi_larger.y -= 5;
            roi_larger.height += 10;
            roi_larger.width += 10;

            Mat unknownImg;
            unknownImg = frame(roi_larger);

            vector < Mat> comparisonResults(9);

            for (int i = 0; i < 9; i++)
            {
                Mat testImgResize;

                if(rects[x].roi.height <= 0 || rects[x].roi.width <=0)
                {
                    cout << "bad rectangle" << endl << rects[x].roi.height << " " << rects[x].roi.width << endl;
                    return;
                }

                resize(testImgs[i], testImgResize, rects[x].roi.size());

                matchTemplate(unknownImg, testImgResize, comparisonResults[i], TM_CCOEFF_NORMED);
                //TODO make sure this cant crash & that CCOEFF provides the best results
                //for optimization, you could implement two match templates, & lower the resolution.
            }

            double min, max;
            double vectorMax = -1;
            int pos = -1;

            for (int i = 0; i < 9; i++)
            {
                minMaxLoc(comparisonResults[i], &min, &max);

                if (vectorMax < max) {
                    vectorMax = max;
                    pos = i;
                }
                rects[x].confidences.push_back(max);
            }
            rects[x].guessedNum = pos + 1;
        }
        gridErrorChecking2(rects);
    }
    // --------------- finish number recognition -----------------/>


    if(rects.size() == 9)
    {
        bool checkIfEqual = true;
        for (int i = 0; i < 9; i++)
        {
            if (lastValidGuessedNums[i] != rects[i].guessedNum || lastValidGuessedNums[i] == 0)
                checkIfEqual = false;
        }

        if (checkIfEqual)
            numberOfValidConsecutiveFrames++;
        else
            numberOfValidConsecutiveFrames = 0;

        for (int i = 0; i < 9; i++)
        {
            lastValidGuessedNums[i] = rects[i].guessedNum;
            returnGrid[i] = rects[i].guessedNum;
        }
    }
    else
        numberOfValidConsecutiveFrames = 0;


    if(numberOfValidConsecutiveFrames >= 3)
        readyToFire = true;
    else
        readyToFire = false;


    // ----------------draw rects ------------
    for (int i = 0; i < rects.size(); i++)
    {
        rectangle(frame, rects[i].roi, Scalar(255, 255,0), 2);

        if (success >= numSuccessFrames)
        {
            string guessedNum = to_string(rects[i].guessedNum);
            putText(frame, guessedNum, Point(rects[i].X() + rects[i].roi.width, rects[i].Y() + 35), FONT_HERSHEY_DUPLEX, 1.5, Scalar(230, 0, 255), 2);
        }
        if (readyToFire)
        {
            rectangle(frame, rects[i].roi, Scalar(55, 55, 255), 3);
            //if( rects[i].guessedNum == targetNum)
            //    putText(frame, "FIRE NOW", Point(rects[i].X() + rects[i].roi.width, rects[i].Y() + 35), FONT_HERSHEY_DUPLEX, 1.5, Scalar(230, 0, 255), 2);
        }

        /*
        string vertConfs = "" + to_string(rects[i].vertConfs);
        string horizConfs= "" + to_string(rects[i].horizConfs);
        putText(frame, vertConfs, Point(rects[i].X(), rects[i].Y() + 14), FONT_HERSHEY_PLAIN, 1, Scalar(10, 0, 255),2);
        putText(frame, horizConfs, Point(rects[i].X(), rects[i].Y() + 30), FONT_HERSHEY_PLAIN, 1, Scalar(10, 0, 255),2);
        */
    }

    namedWindow("after process");
    imshow("after process", frame);
    waitKey(30);
}
