#pragma once
#include "stdafx.h"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#include <opencv2/ml.hpp>
#include <opencv2/opencv.hpp>

using namespace cv::ml;

//for DJI dataset
/*
void load_images(const string & dirname,
                       vector< Mat > & img_lst,
                       bool showImages = false);
*/

void numPredictorSetup(Ptr <SVM> & model);

Mat deskew(Mat& img);

void loadTrainTestLabelDJIdataset(std::vector<Mat> &trainCells,
    vector<Mat> &testCells,
    vector<int> &trainLabels,
    vector<int> &testLabels,
    int & ImgCount);

void loadTrainTestLabel(string &pathName,
    vector<Mat> &trainCells,
    vector<Mat> &testCells,
    vector<int> &trainLabels,
    vector<int> &testLabels);

void CreateDeskewedTrainTest(vector<Mat> &deskewedTrainCells,
    vector<Mat> &deskewedTestCells,
    vector<Mat> &trainCells,
    vector<Mat> &testCells);

void CreateTrainTestHOG(vector<vector<float> > &trainHOG,
    vector<vector<float> > &testHOG,
    vector<Mat> &deskewedtrainCells,
    vector<Mat> &deskewedtestCells);

void ConvertVectortoMatrix(vector<vector<float> > &trainHOG,
    vector<vector<float> > &testHOG,
    Mat &trainMat, Mat &testMat);

void getSVMParams(SVM *svm);

Ptr<SVM> svmInit(float C, float gamma);

void svmTrain(Ptr<SVM> svm, Mat &trainMat, vector<int> &trainLabels);

void svmPredict(Ptr<SVM> svm, Mat &testResponse, Mat &testMat);

void SVMevaluate(Mat &testResponse, float &count,
                      float &accuracy,
                      vector<int> &testLabels);

vector<int> digit_rec(vector<Mat> src_lst, Ptr<SVM> svm);

//TODO split this up into a function that runs early and the regular bit
int mainNumPredictor(std::vector < Mat > Rects, vector <int> & returnGrid);