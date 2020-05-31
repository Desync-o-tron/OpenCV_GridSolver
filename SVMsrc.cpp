#include "stdafx.h"
#include "SVMsrc.h"

string pathName = "digits.png";
int SZ = 20;
float affineFlags = WARP_INVERSE_MAP | INTER_LINEAR;

void load_images(const string & dirname,
    vector< Mat > & img_lst,
    bool showImages = false)
{
    vector< String > files;
    glob(dirname, files);
    //cout << "files.size  " << files.size() << endl;
    for (size_t i = 0; i < files.size(); ++i)
    {
        Mat img = imread(files[i]); // load the image
        if (img.empty())            // invalid image, skip it.
        {
            cout << files[i] << " is invalid!" << endl;
            continue;
        }
        if (showImages)
        {
            imshow("image", img);
            waitKey(100);
        }
        //cvtColor(img, img, CV_BGR2GRAY);
        //resize(img, img, Size(20, 20));
        img_lst.push_back(img);
    }
}

Mat deskew(Mat& img) {
    Moments m = moments(img);
    if (abs(m.mu02) < 1e-2) {
        return img.clone();
    }
    float skew = m.mu11 / m.mu02;
    Mat warpMat = (Mat_<float>(2, 3) << 1, skew, -0.5*SZ*skew, 0, 1, 0);
    Mat imgOut = Mat::zeros(img.rows, img.cols, img.type());
    warpAffine(img, imgOut, warpMat, imgOut.size(), affineFlags);

    return imgOut;
}

void loadTrainTestLabelDJIdataset(vector<Mat> &trainCells, vector<Mat> &testCells, vector<int> &trainLabels, vector<int> &testLabels, int & ImgCount)
{

    vector < Mat > img_lst;
    vector < vector < Mat > > img_arr;

    cout << endl;
    string lol = "loading images";
    cout << lol;

    int numAmt = 1;
    for (int z = 1; z <= numAmt; z++)
    {
        string folder = "REALnumber/number/";
        folder += to_string(z);
        folder += "_";
        //cout << folder << endl;
        load_images(folder, img_lst, false);
        img_arr.push_back(img_lst);
        img_lst.clear();

        cout << "\r" << lol;
        lol += ".";
    }
    cout << endl;

    for (int r = 0; r < numAmt; r++) {
        for (int x = 0; x < img_arr[r].size(); x++) {

            cvtColor(img_arr[r][x], img_arr[r][x], CV_BGR2GRAY);
            resize(img_arr[r][x], img_arr[r][x], Size(20, 20));

            if (x % 10 == 9) {
                testCells.push_back(img_arr[r][x]);
                testLabels.push_back(r + 1);
                /*
                cout << "This is a " << testLabels.back() << endl;
                imshow("ddd", testCells.back());
                waitKey(200);
                */
            }
            else {
                trainCells.push_back(img_arr[r][x]);
                trainLabels.push_back(r + 1);
                /*
                cout << "This is a " << trainLabels.back() << endl;
                imshow("ddd", trainCells.back());
                */
            }

            ImgCount++;

        }
    }

}
void loadTrainTestLabel(string &pathName, vector<Mat> &trainCells, vector<Mat> &testCells, vector<int> &trainLabels, vector<int> &testLabels)
{
    Mat img = imread(pathName, CV_LOAD_IMAGE_GRAYSCALE);
    int ImgCount = 0;
    ///*
    for (int i = 0; i < img.rows; i = i + SZ)
    {
        for (int j = 0; j < img.cols; j = j + SZ)
        {
            Mat digitImg = (img.colRange(j, j + SZ).rowRange(i, i + SZ)).clone();
            if (j < int(0.9*img.cols))
            {
                trainCells.push_back(digitImg);
            }
            else
            {
                testCells.push_back(digitImg);
            }
            ImgCount++;
        }
    }

    float digitClassNumber = 0;

    for (int z = 0; z<int(0.9*ImgCount); z++) {
        if (z % 450 == 0 && z != 0) {
            digitClassNumber = digitClassNumber + 1;
        }
        trainLabels.push_back(digitClassNumber);
    }
    digitClassNumber = 0;
    for (int z = 0; z<int(0.1*ImgCount); z++) {
        if (z % 50 == 0 && z != 0) {
            digitClassNumber = digitClassNumber + 1;
        }
        testLabels.push_back(digitClassNumber);
    }
    //*/
    loadTrainTestLabelDJIdataset(trainCells, testCells, trainLabels, testLabels, ImgCount);
    cout << "Image Count : " << ImgCount << endl;
}


void CreateDeskewedTrainTest(vector<Mat> &deskewedTrainCells, vector<Mat> &deskewedTestCells, vector<Mat> &trainCells, vector<Mat> &testCells) {

    for (int i = 0; i<trainCells.size(); i++) {

        Mat deskewedImg = deskew(trainCells[i]);
        deskewedTrainCells.push_back(deskewedImg);
    }

    for (int i = 0; i<testCells.size(); i++) {

        Mat deskewedImg = deskew(testCells[i]);
        deskewedTestCells.push_back(deskewedImg);
    }
}

HOGDescriptor hog(
    Size(20, 20), //winSize
    Size(8, 8), //blocksize
    Size(4, 4), //blockStride, (4,4) og
    Size(8, 8), //cellSize,(8,8) originially //TODO increase, try 16
    9, //nbins,
    1, //derivAper,
    -1, //winSigma,
    0, //histogramNormType,
    0.2, //L2HysThresh,
    0,//gammal correction,
    64,//nlevels=64
    1);

void CreateTrainTestHOG(vector<vector<float> > &trainHOG, vector<vector<float> > &testHOG, vector<Mat> &deskewedtrainCells, vector<Mat> &deskewedtestCells) {

    for (int y = 0; y<deskewedtrainCells.size(); y++) {
        vector<float> descriptors;
        hog.compute(deskewedtrainCells[y], descriptors);
        trainHOG.push_back(descriptors);
    }

    for (int y = 0; y<deskewedtestCells.size(); y++) {

        vector<float> descriptors;
        hog.compute(deskewedtestCells[y], descriptors);
        testHOG.push_back(descriptors);
    }
}
void ConvertVectortoMatrix(vector<vector<float> > &trainHOG, vector<vector<float> > &testHOG, Mat &trainMat, Mat &testMat)
{

    int descriptor_size = trainHOG[0].size();

    for (int i = 0; i<trainHOG.size(); i++) {
        for (int j = 0; j<descriptor_size; j++) {
            trainMat.at<float>(i, j) = trainHOG[i][j];
        }
    }
    for (int i = 0; i<testHOG.size(); i++) {
        for (int j = 0; j<descriptor_size; j++) {
            testMat.at<float>(i, j) = testHOG[i][j];
        }
    }
}

void getSVMParams(SVM *svm)
{
    cout << "Kernel type     : " << svm->getKernelType() << endl;
    cout << "Type            : " << svm->getType() << endl;
    cout << "C               : " << svm->getC() << endl;
    cout << "Degree          : " << svm->getDegree() << endl;
    cout << "Nu              : " << svm->getNu() << endl;
    cout << "Gamma           : " << svm->getGamma() << endl;
}

Ptr<SVM> svmInit(float C, float gamma)
{
    Ptr<SVM> svm = SVM::create();
    svm->setGamma(gamma);
    svm->setC(C);
    svm->setKernel(SVM::RBF);
    svm->setType(SVM::C_SVC);

    return svm;
}

void svmTrain(Ptr<SVM> svm, Mat &trainMat, vector<int> &trainLabels)
{
    Ptr<TrainData> td = TrainData::create(trainMat, ROW_SAMPLE, trainLabels);
    svm->train(td);
    svm->save("./results/eyeGlassClassifierModel.yml");
}

void svmPredict(Ptr<SVM> svm, Mat &testResponse, Mat &testMat)
{
    svm->predict(testMat, testResponse);
}

void SVMevaluate(Mat &testResponse, float &count, float &accuracy, vector<int> &testLabels)
{
    for (int i = 0; i < testResponse.rows; i++)
    {
        // cout << testResponse.at<float>(i,0) << " " << testLabels[i] << endl;
        if (testResponse.at<float>(i, 0) == testLabels[i]) {
            count = count + 1;
            //cout<<testResponse.at<float>(i,0);
        }

    }
    accuracy = (count / testResponse.rows) * 100;
}

vector<int> digit_rec(vector<Mat> src_lst, Ptr<SVM> svm) {
    vector<Mat> resized_lst;
    //------------------------preprocess start------------------
    for (int i = 0; i<src_lst.size(); i++) {
        Mat resized;
        inRange(src_lst[i], Scalar(0), Scalar(60), resized);
        //imshow("jjj", resized);
        Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
        morphologyEx(resized, resized, MORPH_CLOSE, element);
        //create mask
        Mat mask = resized.clone();
        mask.setTo(0);
        circle(mask, Point(mask.cols / 2, mask.rows / 2), mask.rows / 2, Scalar(255), -1);
        resized = resized & mask;
        vector<vector<Point>> cnt;
        findContours(resized, cnt, RETR_LIST, CHAIN_APPROX_NONE);

        //then find rect
        Rect biggest(0, 0, 0, 0);
        for (int j = 0; j<cnt.size(); j++) {
            drawContours(resized, cnt, j, 255, 5);
            Rect curr = boundingRect(cnt[j]);
            if (curr.height*curr.width>biggest.height*biggest.width) {
                biggest = curr;
            }
        }

        //enlarge the area a bit
        biggest.x = biggest.x - biggest.width*.8;
        biggest.y = biggest.y - biggest.height / 5;
        biggest.width += biggest.width*1.2;
        biggest.height += biggest.height / 3;
        if (biggest.x<0)biggest.x = 0;
        if (biggest.y<0)biggest.y = 0;
        if (biggest.width + biggest.x>resized.cols)biggest.width = resized.cols - biggest.x;
        if (biggest.height + biggest.y>resized.rows)biggest.height = resized.rows - biggest.y;


        if (biggest.width == 0 || biggest.height == 0) //luke fix
        {
            cout << "preprocessing jam\n";
            vector <int> failsafe;
            failsafe.push_back(9);
            return failsafe;
        }
        
        resized = resized(biggest);
        resize(resized, resized, Size(90, 90));
        
        findContours(resized, cnt, RETR_LIST, CHAIN_APPROX_NONE);
        for (int j = 0; j<cnt.size(); j++) {
            drawContours(resized, cnt, j, 255, 2);
        }
        //imshow("Pre resized preprocess rst", resized);
        resize(resized, resized, Size(20, 20));
        //imshow("preprocess rst", resized);

        resized_lst.push_back(resized);
    }

    //------------------------preprocess end------------------
    vector<Mat> deskewedTestCells;
    for (int i = 0; i<resized_lst.size(); i++) {

        Mat deskewedImg = deskew(resized_lst[i]);
        deskewedTestCells.push_back(deskewedImg);
    }

    std::vector<std::vector<float> > testHOG;
    for (int y = 0; y<deskewedTestCells.size(); y++) {

        vector<float> descriptors;
        hog.compute(deskewedTestCells[y], descriptors);
        testHOG.push_back(descriptors);
    }
    int descriptor_size = testHOG[0].size();
    Mat testMat(testHOG.size(), descriptor_size, CV_32FC1);
    for (int i = 0; i<testHOG.size(); i++) {
        for (int j = 0; j<descriptor_size; j++) {
            testMat.at<float>(i, j) = testHOG[i][j];
        }
    }
    Mat testResponse;
    svm->predict(testMat, testResponse);
    vector<int> rtn;
    for (int i = 0; i < testResponse.rows; i++)
    {
        // cout << testResponse.at<float>(i,0) << " " << testLabels[i] << endl;
        rtn.push_back(testResponse.at<float>(i, 0));


    }
    return rtn;
}

void numPredictorSetup(Ptr <SVM> & model)
{
    vector<Mat> trainCells;
    vector<Mat> testCells;
    vector<int> trainLabels;
    vector<int> testLabels;
    loadTrainTestLabel(pathName, trainCells, testCells, trainLabels, testLabels);

    vector<Mat> deskewedTrainCells;
    vector<Mat> deskewedTestCells;
    CreateDeskewedTrainTest(deskewedTrainCells, deskewedTestCells, trainCells, testCells);

    std::vector<std::vector<float> > trainHOG;
    std::vector<std::vector<float> > testHOG;
    CreateTrainTestHOG(trainHOG, testHOG, deskewedTrainCells, deskewedTestCells);

    int descriptor_size = trainHOG[0].size();
    cout << "Descriptor Size : " << descriptor_size << endl;

    Mat trainMat(trainHOG.size(), descriptor_size, CV_32FC1);
    Mat testMat(testHOG.size(), descriptor_size, CV_32FC1);

    ConvertVectortoMatrix(trainHOG, testHOG, trainMat, testMat);

    float C = 12.5, gamma = 0.5;

    Mat testResponse;
    model = svmInit(C, gamma);

    ///////////  SVM Training  ////////////////
    svmTrain(model, trainMat, trainLabels);

    ///////////  SVM Testing  ////////////////
    svmPredict(model, testResponse, testMat);

    ////////////// Find Accuracy   ///////////
    float count = 0;
    float accuracy = 0;
    getSVMParams(model);
    SVMevaluate(testResponse, count, accuracy, testLabels);

    cout << "the accuracy is :" << accuracy << endl;

    return;
}

int mainNumPredictor(std::vector < Mat > Rects, vector <int> & returnGrid)
{

    vector<Mat> trainCells;
    vector<Mat> testCells;
    vector<int> trainLabels;
    vector<int> testLabels;
    loadTrainTestLabel(pathName, trainCells, testCells, trainLabels, testLabels);

    vector<Mat> deskewedTrainCells;
    vector<Mat> deskewedTestCells;
    CreateDeskewedTrainTest(deskewedTrainCells, deskewedTestCells, trainCells, testCells);

    std::vector<std::vector<float> > trainHOG;
    std::vector<std::vector<float> > testHOG;
    CreateTrainTestHOG(trainHOG, testHOG, deskewedTrainCells, deskewedTestCells);

    int descriptor_size = trainHOG[0].size();
    cout << "Descriptor Size : " << descriptor_size << endl;

    Mat trainMat(trainHOG.size(), descriptor_size, CV_32FC1);
    Mat testMat(testHOG.size(), descriptor_size, CV_32FC1);

    ConvertVectortoMatrix(trainHOG, testHOG, trainMat, testMat);

    float C = 12.5, gamma = 0.5;

    Mat testResponse;
    Ptr<SVM> model = svmInit(C, gamma);

    ///////////  SVM Training  ////////////////
    svmTrain(model, trainMat, trainLabels);

    ///////////  SVM Testing  ////////////////
    svmPredict(model, testResponse, testMat);

    ////////////// Find Accuracy   ///////////
    float count = 0;
    float accuracy = 0;
    getSVMParams(model);
    SVMevaluate(testResponse, count, accuracy, testLabels);

    cout << "the accuracy is :" << accuracy << endl;

    while (!Rects.empty()) {
        /*
        cout << "Press any key + Enter. Type [end] to end." << endl;
        string filename;
        cin >> filename;
        if (filename == "end") {
            break;
        }
        */
        Mat imagein;
        imagein = Rects.back();
        Rects.pop_back();
        if (!imagein.data)                              // Check for invalid input
        {
            cout << "Could not open or find the image" << std::endl;
            continue;
        }
        resize(imagein, imagein, Size(160, 160));
        vector<Mat> img_lst;
        img_lst.push_back(imagein);
        vector<int> rst_lst = digit_rec(img_lst, model);
        //imshow("Image I see", imagein);
        cout << "I think it is a " << rst_lst[0] << ".\n";
        //waitKey(27);
        returnGrid.push_back(rst_lst[0]);
    }
    std::reverse(returnGrid.begin(),returnGrid.end());

    return 0;
}

