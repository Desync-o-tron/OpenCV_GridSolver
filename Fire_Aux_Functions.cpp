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
    //TODO weights
    throw(1);
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
    return true;
}