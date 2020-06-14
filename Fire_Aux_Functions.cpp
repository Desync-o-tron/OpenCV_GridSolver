#include "stdafx.h"
#include "Fire_Aux_Functions.h"

bool massageImg(Mat& inFrame, Mat& outFrame) {
    outFrame = inFrame.clone();
    cvtColor(outFrame, outFrame, COLOR_BGR2GRAY);

    //blurring action
    Mat element = getStructuringElement(MORPH_RECT, Size(4, 4), Point(0, 0));
    dilate(outFrame, outFrame, element);
    dilate(outFrame, outFrame, element);
    element = getStructuringElement(MORPH_RECT, Size(7, 7), Point(0, 0));
    erode(outFrame, outFrame, element);
    threshold(outFrame, outFrame, 60, 255, THRESH_BINARY);

    return true;
}


bool contours2boundingRects(vector <vector<Point>> & contours, Mat frame, vector <RectStats> & rects) {
   
    for (int i = 0; i < contours.size(); i++)
    {
        float area = (float)(contourArea(contours[i]));
        float imgArea = frame.cols * frame.rows;
        float minAreaMod = 0.00054;
        float maxAreaMod = 0.03;
        //find objects that are larger than this and smaller than that
        if (area > imgArea * minAreaMod && area < imgArea * maxAreaMod)
        {
            RectStats RS;
            RS.roi = boundingRect(contours[i]);

            //check if the dimensions of the bounding box are reasonable
            float h2wRatioMod = 0.62;
            if (RS.roi.height > RS.roi.width*h2wRatioMod)
                rects.push_back(RS);
        }
    } 
    return true; //end sorting by area and dimension ratio
}

bool filterRectsByAdjacency(Mat & frame, vector <RectStats> & rects) {
    
    for (int i = 0; i < rects.size(); i++) {
        rects[i].horizConfs = 0;
        rects[i].vertConfs = 0;
    }

    float areaTolMult = 1.65; //1.50 too small
    float closeTolMult = 0.5;
    int horizTol_dstToNeigh = frame.cols * 0.25;
    int vertTol_dstToNeigh = frame.rows * 0.20;
    Rect roiUp, roiDown, roiLeft, roiRight;

    for (int i = 0; i < rects.size(); i++) {
        //to check if the rectangles are in the same col or row
        int horizTol_inCol = rects[i].roi.width * closeTolMult;
        int vertTol_inRow = rects[i].roi.height * closeTolMult;

        for (int j = 0; j < rects.size(); j++)
        {
            if (j != i)
            {
                roiUp.height = vertTol_dstToNeigh;
                roiUp.width = horizTol_inCol * 2;
                roiUp.y = rects[i].roi.y - roiUp.height;
                roiUp.x = rects[i].roi.x - roiUp.width / 2.0;
                //rectangle(frame, roiUp, Scalar(200, 0, 200));

                roiDown.height = vertTol_dstToNeigh;
                roiDown.width = horizTol_inCol * 2;
                roiDown.y = rects[i].roi.y;// +roiDown.height;
                roiDown.x = rects[i].roi.x - roiDown.width / 2.0;
                //rectangle(frame, roiDown, Scalar(150, 0, 250));

                roiLeft.height = vertTol_inRow * 2;
                roiLeft.width = horizTol_dstToNeigh;
                roiLeft.y = rects[i].roi.y - roiLeft.height / 2.0;
                roiLeft.x = rects[i].roi.x - roiLeft.width;
                //rectangle(frame, roiLeft, Scalar(150, 0, 250));

                roiRight.height = vertTol_inRow * 2;
                roiRight.width = horizTol_dstToNeigh;
                roiRight.y = rects[i].roi.y - roiRight.height / 2.0;
                roiRight.x = rects[i].roi.x;// roiRight.width;
                //rectangle(frame, roiRight, Scalar(150, 0, 250));

                if (rects[i].Area() / areaTolMult <= rects[j].Area() && rects[j].Area() <= rects[i].Area() * areaTolMult)
                {
                    if (roiUp.contains(Point(rects[j].X(), rects[j].Y())))
                        rects[i].vertConfs++;
                    if (roiDown.contains(Point(rects[j].X(), rects[j].Y()))) //TODO else's?
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
    return true; //---------------------- end adjacnency algo ------------------------------------------
}


bool deleteRectanglesNearScreenEdge(Mat& frame, vector <RectStats>& rects) {

    int const pixelsFromEdge = 15;
    for (int i = 0; i < rects.size();)
    {
        if (rects[i].X() < pixelsFromEdge)
            rects.erase(rects.begin() + i);
        else if (rects[i].X() > frame.cols - rects[i].roi.width - pixelsFromEdge)
            rects.erase(rects.begin() + i);
        else if (rects[i].Y() < pixelsFromEdge)
            rects.erase(rects.begin() + i);
        else if (rects[i].Y() > frame.rows - rects[i].roi.height - pixelsFromEdge)
            rects.erase(rects.begin() + i);
        else
            i++;
    }
    return true;
}


bool deleteSmallestTargets(vector <RectStats>& rects) {
    
    sort(rects.begin(), rects.end(),
        [](RectStats const& a, RectStats const& b) { return (a.roi.height * a.roi.width) < (b.roi.height * b.roi.width); });

    while (rects.size() > 9) {
        rects.erase(rects.begin());
    }
    
    return true;
}

bool smallestTargetIsValid(vector <RectStats>& rects) {
    if (rects.begin()->roi.height <= 0 || rects.begin()->roi.width <= 0)
        return false;
    else
        return true;
}


bool orderRectsByPos(vector <RectStats>& rects) {
    // ----- organize grid pos 1 - 9 -----
    if (rects.size() == 9)
    {

        sort(rects.begin(), rects.end(),
            [](RectStats const& a, RectStats const& b) { return a.roi.y < b.roi.y; });

        vector <RectStats> organizedRects;

        for (int a = 0; a < 3; a++)
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

        return true;
    }
    //else
    //{
    //    for (int p = 0; p < rects.size(); p++)
    //    {
    //        rects[p].gridPos = 0;
    //        rects[p].guessedNum = 0;
    //    }
    //}
    return false;
}


bool guessNumbers(Mat& frame, vector <RectStats>& rects, const vector<Mat>& testImgs) 
{
    // --------------------------- match template ---------------------------
    for (int x = 0; x < 9; x++)
    {
        Mat unknownImg;
        vector <Mat> comparisonResults(9); //heatmaps

        //TODO lower these to improve accuracy?
        //enlarge bounding box
        Rect larger_roi = rects[x].roi;
        int pixelIncrease = 10;
        larger_roi.x -= pixelIncrease/2;
        larger_roi.y -= pixelIncrease/2;
        larger_roi.height += pixelIncrease;
        larger_roi.width += pixelIncrease;
        unknownImg = frame(larger_roi);

        for (int i = 0; i < 9; i++)
        {
            Mat testImgResized;
            resize(testImgs[i], testImgResized, rects[x].roi.size());
            matchTemplate(unknownImg, testImgResized, comparisonResults[i], TM_CCOEFF_NORMED);
            //TODO make sure this cant crash & that CCOEFF provides the best results
            //for optimization, you could implement two match templates, & lower the resolution.
        }

        double min, max;
        double maxAcrossImgs = -1;
        int hottestPointIndex = -1;
        //for each heatmap, find the max
        //find the hottest point among all maps
        for (int i = 0; i < 9; i++)
        {
            minMaxLoc(comparisonResults[i], &min, &max);

            if (maxAcrossImgs < max) {
                maxAcrossImgs = max;
                hottestPointIndex = i;
            }
            rects[x].confidences.push_back(max);
        }
        rects[x].guessedNum = hottestPointIndex + 1;
    }
    return true;
}


bool getTargetLock(vector<RectStats>& rects, vector<int> & lastValidGuessedNums, vector<int>& returnGrid, int & numberOfValidConsecutiveFrames)
{
    if (rects.size() == 9)
    {
        bool lastRecsAreTheSame = true;
        for (int i = 0; i < 9; i++)
        {
            if (lastValidGuessedNums[i] != rects[i].guessedNum || lastValidGuessedNums[i] == 0)
                lastRecsAreTheSame = false;
        }

        if (lastRecsAreTheSame)
            numberOfValidConsecutiveFrames++;
        else
            numberOfValidConsecutiveFrames = 0;

        for (int i = 0; i < 9; i++)
        {
            lastValidGuessedNums[i] = rects[i].guessedNum;
            returnGrid[i] = rects[i].guessedNum;
        }
    }
    else { 
        numberOfValidConsecutiveFrames = 0; 
    }
        
    if (numberOfValidConsecutiveFrames >= 3)
        return true;
    else
        return false;
}
