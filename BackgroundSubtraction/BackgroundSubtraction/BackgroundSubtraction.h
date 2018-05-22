#pragma once
#include "BackgroundSubtraction.h"
#include <iostream>
#include "opencv2\highgui\highgui.hpp"
#include "opencv\cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>
#include <array>
#include <opencv/cv.h>
using namespace cv;
using namespace std;

class BackgroundSubtractor {
public:
	BackgroundSubtractor();
	void background_subtraction(const cv::String imageDestination);
	Mat subtractedBackground;

private:
	
	Mat image_;
	Mat processed_;
	Mat canny_;
	Mat edges_;

	int iteration;
	array<int, 10> brightness_{ 100, -80, 100, 100, 50, -150, -170, 50, -150, -80 };
	array<double, 10> contrast_{1, 1.6, 1, 1.6, 1.8, 1.8, 1.4, 1.8, 1.8, 1.6 };
	array<int, 10> median_{ 101, 101, 201, 101, 175, 101, 175, 101, 175, 175 };

	int cannyLow_ = 5;
	int cannyHigh_ = 70;

	Scalar color_ = Scalar(255, 255, 255);
	bool isLeftDefined_;
	bool isRightDefined_;
	bool isBladeDefined_;

	vector<vector<Point>> contours_;
	vector<Vec4i> hierarchy_;
	int largestPerimeter_;
	int largestContourIndex_;
	int secondLargestPerimeter_;
	int secondLargestContourIndex_;

	vector<vector<Point> > leftContours_;
	vector<Vec4i> leftHierarchy_;
	int leftContourIndex_;

	vector<vector<Point> > rightContours_;
	vector<Vec4i> rightHierarchy_;
	int rightContourIndex_;

	bool twoGoodLines_;
	int lineLeft_;
	int lineRight_;

	void image_process();
	void contour_finder();
	void contour_drawer();
	void blade_detector();
	void blade_decision_maker();
	void blade_drawer();
};
