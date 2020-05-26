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


bool contours2boundingRects(vector <vector<Point>> & contours, float imgArea, vector <RectStats> & rects) {
   
    for (int i = 0; i < contours.size(); i++)
    {
        float area = (float)(contourArea(contours[i]));
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
            {
                //uncomment to deal with shitty cameras giving us tiny artifacts
                // if(roi.height >= 8 && roi.width >= 8 &&
                // roi.x > 0 && roi.x < frame.cols &&
                // roi.y > 0 && roi.y < frame.rows)
                rects.push_back(RS);
            }
        }
    } //end sorting by area and dimension ratio
    return true;
}