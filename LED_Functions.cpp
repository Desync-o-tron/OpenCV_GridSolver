#include "stdafx.h"
#include "LED_Functions.h"

/*
void filter1(Mat src, Mat&dst) {
    Mat hsv;
    cvtColor(src, hsv, CV_BGR2HSV);
    //imshow("HSV img",hsv);
    vector<Mat> hsvSpl;
    split(hsv, hsvSpl);
    //equalizeHist(hsvSpl[2],hsvSpl[2]);
    // Mat color;
    //inRange(src, Scalar(10,0,0), Scalar(255,255,255),color);
    inRange(hsvSpl[0], Scalar(90), Scalar(120), hsvSpl[0]);
    inRange(hsvSpl[1], Scalar(0), Scalar(35), hsvSpl[1]);
    inRange(hsvSpl[2], Scalar(240), Scalar(255), hsvSpl[2]);
    // hsvSpl[0]=color;

    imshow("s", hsvSpl[1]);
    imshow("v", hsvSpl[2]);
    Mat snv = hsvSpl[1] & hsvSpl[2];
    Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
    morphologyEx(snv, snv, MORPH_OPEN, element);
    morphologyEx(hsvSpl[0], hsvSpl[0], MORPH_OPEN, element);
    element = getStructuringElement(MORPH_RECT, Size(20, 20));
    morphologyEx(hsvSpl[0], hsvSpl[0], MORPH_CLOSE, element);
    imshow("h", hsvSpl[0]);
    Mat rst = hsvSpl[0] & snv;
    //Mat hnv=hsvSpl[0]&hsvSpl[2];
    //Mat snh=hsvSpl[1]&hsvSpl[0];
    //imshow("h",hsvSpl[0]);
    //imshow("s&v",snv);
    //imshow("rst",rst);
    //imshow("h&v",hnv);
    //imshow("s&h",snh);
    dst = rst;
}

int SingleSevenSegRec(Mat frame) {
    inRange(frame, Scalar(150), Scalar(255), frame);
    //imshow("single seg",frame);
    int rtn = -1;
    const int lookup_table[9][2] =
    { { 0x3f,8 },
    { 0x2f,9 },
    { 0x3d,6 },
    { 0x07,7 },
    { 0x2d,5 },
    { 0x26,4 },
    { 0x0f,3 },
    { 0x1b,2 },
    { 0x06,1 }
    };
    int mark = 0;

    Mat mask;
    frame.copyTo(mask);

    if (frame.rows - 2 * frame.cols>0) {
        return 1;
    }

    ////seg0
    mask.setTo(0);
    line(mask, Point(frame.cols / 2, 0), Point(frame.cols / 2, frame.rows / 3), Scalar(255));
    mask = mask & frame;
    Scalar scount0 = sum(mask);
    int count0 = scount0[0] / 255;
    //cout<<"count0 "<<count0<<endl;
    if (count0>1) {
        mark |= 1;
    }

    //seg3
    mask.setTo(0);
    line(mask, Point(frame.cols / 2 - 2, frame.rows / 3 * 2), Point(frame.cols / 2 - 2, frame.rows), Scalar(255));
    mask = mask & frame;
    Scalar scount3 = sum(mask);
    int count3 = scount3[0] / 255;
    //cout<<"count3 "<<count3<<endl;
    if (count3>1) {
        mark |= 1 << 3;
    }

    ////seg1
    mask.setTo(0);
    line(mask, Point(frame.cols / 2, frame.rows / 3), Point(frame.cols, frame.rows / 3), Scalar(255));
    mask = mask & frame;
    //imshow("mask", mask);
    Scalar scount1 = sum(mask);
    int count1 = scount1[0] / 255;
    //cout<<"count1 "<<count1<<endl;
    if (count1>1) {
        mark |= 1 << 1;
    }

    ////seg2
    mask.setTo(0);
    line(mask, Point(frame.cols / 2, frame.rows / 3 * 2), Point(frame.cols, frame.rows / 3 * 2), Scalar(255));
    mask = mask & frame;
    //imshow("mask", mask);
    Scalar scount2 = sum(mask);
    int count2 = scount2[0] / 255;
    //cout<<"count2 "<<count2<<endl;
    if (count2>1) {
        mark |= 1 << 2;
    }

    ////seg4
    mask.setTo(0);
    line(mask, Point(0, frame.rows / 3 * 2), Point(frame.cols / 2, frame.rows / 3 * 2), Scalar(255));
    mask = mask & frame;
    //imshow("mask", mask);
    Scalar scount4 = sum(mask);
    int count4 = scount4[0] / 255;
    //cout<<"count4 "<<count4<<endl;
    if (count4>1) {
        mark |= 1 << 4;
    }

    ////seg5
    mask.setTo(0);
    line(mask, Point(0, frame.rows / 3), Point(frame.cols / 2, frame.rows / 3), Scalar(255));
    mask = mask & frame;
    //imshow("mask", mask);
    Scalar scount5 = sum(mask);
    int count5 = scount5[0] / 255;
    //cout<<"count5 "<<count5<<endl;
    if (count5>1) {
        mark |= 1 << 5;
    }



    for (int i = 0; i<9; i++) {
        if (mark == lookup_table[i][0]) {
            rtn = lookup_table[i][1];
        }
    }
    return rtn;
}


//@param frame
//source frame that has single channel
//@return the five numbers

vector<int> SevenSegRec(Mat frame) {
    vector<int> rtn;
    if (frame.channels() != 1) {
        return rtn;
    }
    resize(frame, frame, Size(320, 240));
    Mat equalized;
    equalizeHist(frame, equalized);
    //imshow("equal", equalized);
    Mat binary;

    inRange(equalized, Scalar(250), Scalar(255), binary);
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(binary, binary, MORPH_CLOSE, element);
    //imshow("binary", binary);//see the image in binary
    vector<vector<Point>> contours;
    findContours(binary, contours, RETR_LIST, CHAIN_APPROX_NONE);
    vector<Rect> rects;
    for (int i = 0; i<contours.size(); i++) {
        if (contours[i].size()<30) {
            continue;
        }
        Rect rect;
        rect = boundingRect(contours[i]);
        rects.push_back(rect);

    }
    ///////////here we remove the rectangle of wrong ratio from the list
    for (int i = 0; i<rects.size(); i++) {
        if (rects[i].height - rects[i].width<0) {
            rects.erase(rects.begin() + i);
            //continue;
        }
        //here is demonstration for debugging purpose
        //rectangle(frame,rects[i], Scalar(255));
        //imshow("demo", frame);
    }
    //////////now we find possible regions that has neighbors
    //stack<Rect> tmp;
    vector<Rect> list_of_five;

    inRange(equalized, Scalar(180), Scalar(255), binary);
    element = getStructuringElement(MORPH_RECT, Size(2, 2));
    morphologyEx(binary, binary, MORPH_OPEN, element);
    element = getStructuringElement(MORPH_RECT, Size(8, 8));
    morphologyEx(binary, binary, MORPH_CLOSE, element);
    element = getStructuringElement(MORPH_RECT, Size(12, 12));
    morphologyEx(binary, binary, MORPH_OPEN, element);
    //imshow("binary2", binary);//see the image in binary

    findContours(binary, contours, RETR_LIST, CHAIN_APPROX_NONE);
    vector<Rect> boundingrects;
    for (int i = 0; i<contours.size(); i++) {
        if (contours[i].size()<20) {
            continue;
        }
        Rect rect;
        rect = boundingRect(contours[i]);
        boundingrects.push_back(rect);

    }
    for (int i = 0; i<boundingrects.size(); i++) {
        if ((double)boundingrects[i].height / boundingrects[i].width>.4) {
            boundingrects.erase(boundingrects.begin() + i);
            continue;
        }
        //here is demonstration for debugging purpose
        // rectangle(frame,boundingrects[i], Scalar(255));

    }
    //search seven seg rects in boundingrects
    for (int i = 0; i<boundingrects.size(); i++) {
        boundingrects[i].x -= boundingrects[i].height / 2;
        boundingrects[i].y -= boundingrects[i].height / 2;
        if (boundingrects[i].y<0 || boundingrects[i].x<0) {
            continue;
        }
        boundingrects[i].height += boundingrects[i].height / 2 + 2;
        boundingrects[i].width += boundingrects[i].height / 2 + 2;
        if (boundingrects[i].height + boundingrects[i].y>frame.rows) {
            continue;
        }
        if (boundingrects[i].width + boundingrects[i].x>frame.cols) {
            continue;
        }
        for (int j = 0; j<rects.size(); j++) {
            if (boundingrects[i].contains(rects[j].br()) && boundingrects[i].contains(rects[j].tl())) {
                list_of_five.push_back(rects[j]);
            }
        }
        //rectangle(frame,boundingrects[i], Scalar(255));
        if (list_of_five.size() == 5) {
            break;
        }
        list_of_five.clear();

    }
    //now order the list according to coordinate
    for (int i = 0; i<list_of_five.size(); i++) {
        for (int j = i; j<list_of_five.size(); j++) {
            if (list_of_five[i].x>list_of_five[j].x) {
                Rect tmp = list_of_five[i];
                list_of_five[i] = list_of_five[j];
                list_of_five[j] = tmp;
            }
        }
    }

    for (int i = 0; i<list_of_five.size(); i++) {
        Mat tmp;
        frame(list_of_five[i]).copyTo(tmp);
        int get = SingleSevenSegRec(tmp);
        if (get == -1) {
            rtn.clear();
            return rtn;
        }
        for (int j = 0; j<rtn.size(); j++) {
            if (get == rtn[j]) {
                rtn.clear();
                return rtn;
            }
        }
        rtn.push_back(get);
        //rectangle(frame,list_of_five[i], Scalar(255));
    }
    //////////see through the lst of mats and return numbers

    //imshow("demo", frame);
    return rtn;
}


int LEDSubMain(Mat frame) {
    vector<int> last;

    cvtColor(frame, frame, CV_BGR2GRAY);
    vector<int> rst;
    rst = SevenSegRec(frame);
    if (rst.size() == 5) {
        if (last.size() == 0) {
            cout << rst[0] << rst[1] << rst[2] << rst[3] << rst[4] << endl;
            last.push_back(rst[0]);
            last.push_back(rst[1]);
            last.push_back(rst[2]);
            last.push_back(rst[3]);
            last.push_back(rst[4]);
        }
        else {

            for (int i = 0; i<5; i++) {
                if (rst[i] != last[i]) {
                    cout << rst[0] << rst[1] << rst[2] << rst[3] << rst[4] << endl;
                    last.clear();
                    last.push_back(rst[0]);
                    last.push_back(rst[1]);
                    last.push_back(rst[2]);
                    last.push_back(rst[3]);
                    last.push_back(rst[4]);
                    last = rst;
                    break;
                }
            }
        }
    }
    imshow("frame", frame);
    if ((waitKey(0) & 0xff) == 27) {
    //    break;
    }
    
    //cout << "RoboGrinders Wins" << endl;
    return 0;
}
*/
