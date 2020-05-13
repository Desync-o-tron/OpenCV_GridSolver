#include "stdafx.h"
#include "HandWritingMain.hpp"

void HandWritingMain(Mat & frame, Ptr<SVM> & model, vector <int> & returnGrid, bool & readyToFire)
{

    if (frame.empty() || frame.cols == 0)
    {
        cout << "bad frame";
        return;
    }

   
    //resize(frame, frame, Size(), 0.5, 0.5);
    static int testFireFrameCount;
    static int testFrameCount;
    testFrameCount++;
    
    if (testFrameCount % 10 == 1 && testFireFrameCount != 0) 
        cout << testFrameCount << "\t" << (double)(testFireFrameCount / (1.0*testFrameCount)*100.0) << endl;

    // x + 1
    //1001    24.0759 @ 9
    //1001    25.5744 @ 8
    //1001  22.8771 @ size 7
    //1001    22.3776 @ 6
    //1001    20.2797 @ 5
    // x + 0
    // 1001    21.978 @ 9
    // 1001    24.3756 @ 8
    // x + 2
    //1001    22.2777 @ 9
    //1001    26.1738 @ 8
    //x+3
    // 1001    22.5774 @ 8
    //////////
    //double dilate // all x's are on the first dilate now, the errode takes x+2
    //x/1.4
    //1001    24.0759 @8
    // x/1.5
    //1001    24.0759 @8
    // x/2
    //1001    20.8791 @ 8
    // x/2.5
    //1001    9.79021 @8
    // double dilate, double errode
    // x/1.5 , (x+2)/1.5
    //1001    24.5754 @ 8
    // x/1.5 , (x+0)/1.5
    //1001    21.978@8
    // x/1.3 , (x+0)/1.3
    //shit


    static int success = 0;
    const int _numSuccessFrames = 2;
    
    const int _numFramesOfIdenticalNumbers = 3;
    static int numFramesOfIdenticalNumbers;

    Mat OpFrame, Drawframe;
    Mat duplFrame = frame.clone();

    OpFrame = frame.clone();
    Mat background(duplFrame.rows, duplFrame.cols, CV_8UC3);
    background.setTo(Scalar(255, 255, 255));

    cvtColor(duplFrame, duplFrame, COLOR_BGR2GRAY);

    //dilation for cropping with contours

    int elemSize = frame.rows / (60.0);

    Mat element;    
    element = getStructuringElement(MORPH_RECT, Size(elemSize, elemSize), Point(0, 0));
    dilate(duplFrame, duplFrame, element);
    //dilate(duplFrame, duplFrame, element);
    element = getStructuringElement(MORPH_RECT, Size( (elemSize+2), ( (elemSize + 2)) ), Point(0, 0));
    erode(duplFrame, duplFrame, element);
    //erode(duplFrame, duplFrame, element);

//    imshow("post dilation", duplFrame);
//    waitKey(10);

    //TODO why do we do this min max for canny??
    
    double min, max;
    minMaxLoc(duplFrame, &min, &max);
    Canny(duplFrame, duplFrame, 0.1 * min, 0.9 * max, 3, true);
    //Canny(duplFrame, duplFrame, 0.1 * min, 0.9 * max, 5, true); //5 apature size is worse than 3
    
   // Canny(duplFrame, duplFrame, 10, 100, 3, true);


    std::vector< std::vector< Point > > contours;
    //findContours(canny, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    //findContours(canny, contours, RETR_CCOMP, CHAIN_APPROX_TC89_L1);
    findContours(duplFrame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //CHAIN_APPROX_SIMPLE
    //findContours(canny, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
    //for (int i = 0; i < contours.size(); i++)
       // drawContours(frame, contours, i, Scalar(0,0,255),3);

   // imshow("postThresh", duplFrame);
   // waitKey(10);
        
    //------ filtering by geometry -------------
    std::vector < std::vector <Point > > desiredContours;
    std::vector < Point > cnt;
    vector <HWRectStats> rects;

    for (int i = 0; i < contours.size(); i++)
    {
        cnt = contours[i];
        //filter by rectangle size
        float area = float(contourArea(cnt));
        if (area > frame.cols*frame.rows*0.0048 && area < frame.cols*frame.rows*0.15)
        {
            HWRectStats rect;
            rect.roi = boundingRect(cnt);
            rectangle(Drawframe, rect.roi, Scalar(0, 255, 255)); //yellow

            //filter by rectangle-ness
            float areaRatio = float(rect.roi.area() / (1.0*area));
            if (areaRatio < 1.6) //1.2 is very tight fit //1.5 is ok
            {
                rectangle(Drawframe, rect.roi, Scalar(255, 0, 255)); //le purp
                
                //filter by aspect ratio
                float aspectRatio = float(rect.roi.height / (1.0*rect.roi.width));
                //if (aspectRatio > 0.4 && aspectRatio < 0.7 && rect.roi.height > 5 && rect.roi.width > 5)
                if (aspectRatio > 0.5 && aspectRatio < 0.8 && rect.roi.height > 5 && rect.roi.width > 5)
                {
                    //---- copy contours onto background image --
                    desiredContours.clear();
                    desiredContours.push_back(cnt);

                    Mat mask = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
                    vector< vector<Point> > dst;

                    scaleContours(desiredContours, dst, (float)(0.91));
                    drawContours(mask, dst, -1, Scalar(255), CV_FILLED);

                    OpFrame.copyTo(background, mask);
                    
                    if (rect.roi.height >= 1 && rect.roi.width >= 1)
                        rects.push_back(rect);

                }
            }
        }
    }

    //----------start adjacency algo ------------------------------------------
    if (rects.size() >= 9)
    {
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
                    roiUp.width = horizTol_inCol * 2;
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
    }
    //---------------------- end adjacnency algo ------------------------------------------
    

    //------- get rid of bad rectangles that made it this far --------------------
    while (rects.size() > 9)
    {
        //delete rectangles 10pix from screen edge
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

        if (rects.size() <= 9)
            break;

        //  delete smallest rectangles
        sort(rects.begin(), rects.end(),
            [](HWRectStats const& a, HWRectStats const& b) { return (a.roi.height*a.roi.width) < (b.roi.height*b.roi.width); });
        rects.erase(rects.begin() + 0);

    }
    // ------------------ All deletion finished-------------------------------------------/>

    // print what rects exist for the user
    for (int i = 0; i < rects.size(); i++)
        rectangle(frame, rects[i].roi, Scalar(0, 255, 0), 2); //green

    //---- check for confirmed frames
    if (rects.size() == 9)
        success++;
    else
        success = 0;

    //---- prep the sending of the frames to the digit SVM
    if (success > _numSuccessFrames)
    {
        //---------- assign grid positions  -------------------------
        vector <grid_pos> pseudoRects(9);
        for (int i = 0; i < 9; i++)
        {
            pseudoRects[i].posInOGvector = i;
            pseudoRects[i].pos = Point(rects[i].roi.x, rects[i].roi.y);
        }
        
        vector <grid_pos> tempRects;// (9);
        for (int i = 0; i < 3; i++) 
        {
            sort(pseudoRects.begin(), pseudoRects.end(),
                [](grid_pos const& a, grid_pos const& b) { return a.pos.y < b.pos.y; });

            vector <grid_pos> row;

            for (int a = 0; a < 3; a++) {
                row.push_back(pseudoRects[0]);
                pseudoRects.erase(pseudoRects.begin()+0);
            }

            sort(row.begin(), row.end(),
                [](grid_pos const& a, grid_pos const& b) { return a.pos.x < b.pos.x; });

            for (int a = 0; a < 3; a++) {
                tempRects.push_back(row[a]);
            }

        }
        for (int i = 0; i < 9; i++)
            rects[tempRects[i].posInOGvector].gridPos = i + 1;

        //------------------------- grid positions given -----------------------/>

        
        // ---- start preprocessing the imgs ------------->
//        imshow("initBgnd", background);

        cvtColor(background, background, COLOR_BGR2GRAY);

        int elemSize2 = frame.rows / (160.0);
        element = getStructuringElement(MORPH_ELLIPSE, Size(elemSize2, elemSize2), Point(0, 0));
        erode(background, background, element);
        erode(background, background, element);
        //erode(background, background, element);
        
//        imshow("bkrnd", background);

        minMaxLoc(background, &min, &max);
        threshold(background, background, 170, 255, THRESH_BINARY);
        dilate(background, background, element);

//        imshow("bkrndX", background);
//        waitKey(10);

        for (int i = 0; i < rects.size(); i++)
        {


            Mat img(background(rects[i].roi));
            rects[i].img = img;
            resize(rects[i].img, rects[i].img, Size(180, 180));

            /*
            resize(rects[i].img, rects[i].img, Size(180, 180));

            cvtColor(rects[i].img, rects[i].img, COLOR_BGR2GRAY);

            element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5), Point(0, 0));
            erode(rects[i].img, rects[i].img, element);
            erode(rects[i].img, rects[i].img, element);
            erode(rects[i].img, rects[i].img, element);

            minMaxLoc(rects[i].img, &min, &max);
            threshold(rects[i].img, rects[i].img, 0.455*max, 255, THRESH_BINARY);

            dilate(rects[i].img, rects[i].img, element);
            */
        }
        // ---------- finished preprocessing img --------/>

        // ---------- send to SVM -------        
        for (int i = 0; i < rects.size(); i++)
        {
            vector<Mat> img_lst;
            img_lst.push_back(rects[i].img);
            vector<int> rst_lst = digit_rec(img_lst, model);
            rects[i].guessedNum = rst_lst.back();
        }

        // -- ERROR CHECK --
        vector <gridNum> rectsToErrorCheck(9);
        for (int i = 0; i < 9; i++)
        {
            if (rects[i].guessedNum == 0)
            {
             //   cout << "zero emmitted from svm\n";
              //  waitKey(0);
            }
            rectsToErrorCheck[i].guessedNum = rects[i].guessedNum;
            rectsToErrorCheck[i].gridPos = rects[i].gridPos;
        }
    
        gridErrorChecking(rectsToErrorCheck, false);

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if(rects[i].gridPos == rectsToErrorCheck[j].gridPos)
                    rects[i].guessedNum = rectsToErrorCheck[j].guessedNum;
            }
        }

        bool checkIfEqual = true;
        for (int i = 0; i < 9; i++)
        {
            if (returnGrid[i] != rects[i].guessedNum)
                checkIfEqual = false;
        }

        if (checkIfEqual)
            numFramesOfIdenticalNumbers++;
        else
            numFramesOfIdenticalNumbers = 0;

        for (int i = 0; i < 9; i++)
            returnGrid[i] = rects[i].guessedNum;

        if (numFramesOfIdenticalNumbers > _numFramesOfIdenticalNumbers)
        {
            readyToFire = true;
            testFireFrameCount++;
        }
        else
            readyToFire = false;

        // send data to screen
        for (int i = 0; i < rects.size(); i++)
        {   
            string guessedNum = to_string(rects[i].guessedNum);
            putText(frame, guessedNum, Point(rects[i].X() + rects[i].roi.width, rects[i].Y() + 35), FONT_HERSHEY_DUPLEX, 1.1, Scalar(230, 0, 255), 2);
                
            if (readyToFire == true)
            {
                rectangle(frame, rects[i].roi, Scalar(55, 55, 255), 3);
            }

        }

    }
}