#include "stdafx.h"
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

BackgroundSubtractor::BackgroundSubtractor() {}

void BackgroundSubtractor::background_subtraction(const cv::String imageDestination) {

	image_ = imread(imageDestination, 1);
	leftContourIndex_ = 0;
	rightContourIndex_ = 0;
	twoGoodLines_ = false;
	isBladeDefined_ = false;
	isLeftDefined_ = false;
	isRightDefined_ = false;
	iteration = 0;

	for (int k = 0; k <= (median_.size() - 1); k++) {
		//std::cout << "Median: "<< median_[iteration] << endl;
		lineLeft_ = 0;
		lineRight_ = 0;
		image_process();
		contour_finder();
		contour_drawer();
		blade_detector();
		blade_decision_maker();
		if (isBladeDefined_ == true)
			break;
		iteration = iteration + 1;
	}
		subtractedBackground = Mat::zeros(image_.size(), CV_8UC3);
		blade_drawer();
		cv::imwrite("Results//final.jpg", subtractedBackground);
	

}

void BackgroundSubtractor::image_process() {
	image_.convertTo( processed_, -1, contrast_[iteration], brightness_[iteration]); //adjusting brightness and contrast
	cv::cvtColor(processed_, processed_, CV_BGR2GRAY); //convert to grayscale
	std::cout << endl << "Applying median filter..." << endl; 
	cv::medianBlur(processed_, processed_, median_[iteration]); //median filter
	std::cout << "Median filter applied. " << endl;
	cv::Canny(processed_, canny_, cannyLow_, cannyHigh_, 3); //Canny edge detection
}

void BackgroundSubtractor::contour_finder() {
	/// Find contours   
	cv::RNG rng(12345);
	cv::findContours(canny_, contours_, hierarchy_, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	/// Draw contours
	edges_ = cv::Mat::zeros(canny_.size(), CV_8UC3);

	largestPerimeter_ = 0;
	largestContourIndex_ = 0;
	secondLargestPerimeter_ = 0;
	secondLargestContourIndex_ = 0;

	for (int i = 0; i < contours_.size(); i++) // iterate through each contour. 
	{
		double a = cv::arcLength(contours_[i], false);	//double b = arcLength(contours[i], false);
		if (a > largestPerimeter_) {
			secondLargestPerimeter_ = largestPerimeter_;
			secondLargestContourIndex_ = largestContourIndex_;
			largestPerimeter_ = a;
			largestContourIndex_ = i;              //Store the index of largest contour

		}
		else if ((a < largestPerimeter_) && (a > secondLargestPerimeter_)) {
			secondLargestPerimeter_ = a;
			secondLargestContourIndex_ = i;
		}

	}
}

void BackgroundSubtractor::contour_drawer(){
	largestPerimeter_ = cv::arcLength(contours_[largestContourIndex_], false);
	secondLargestPerimeter_ = arcLength(contours_[secondLargestContourIndex_], false);

	double X1 = contours_[largestContourIndex_][1].x;
	double X2 = contours_[secondLargestContourIndex_][1].x;
	double distance = abs(X1 - X2);

	cv::drawContours(edges_, contours_, largestContourIndex_, color_, 1, 8, hierarchy_, 0, Point());
	if ((secondLargestPerimeter_ > largestPerimeter_ / 3) && (distance > (edges_.cols / 4))) {
		cv::drawContours(edges_, contours_, secondLargestContourIndex_, color_, 1, 8, hierarchy_, 0, Point());
		std::cout << "Second largest accepted" << endl;
	}
	
	cv::cvtColor(edges_, edges_, CV_BGR2GRAY);
	cv::threshold(edges_, edges_, 220, 255, cv::THRESH_BINARY);

}

void BackgroundSubtractor::blade_detector() {


	int half_cols = edges_.cols / 2;


	for (int g = 0; g < edges_.cols; g++) {

		//std::cout << "Second loop Iteration: " << g << endl;
		//std::cout << "Columns: " << drawing.cols << endl;
		//std::cout << "Rows: " << drawing.rows << endl;
		//Scalar a = drawing.at<uchar>(0, g);
		//std::cout << "Color of pixel: " << a << endl;
		//std::cout << "White line left: " << white_line_left << endl;

		if ((edges_.at<uchar>(0, g) == 255) && (g <= half_cols)) {
			lineLeft_ = lineLeft_ + 1;
			std::cout << "White line top left detected." << endl;
		}
		else if ((edges_.at<uchar>(0, g) == 255) && (g > half_cols)) {
			lineRight_ = lineRight_ + 1;
			std::cout << "White line top right detected." << endl;
		}
		if ((edges_.at<uchar>(edges_.rows - 1, g) == 255) && (g <= half_cols)) {
			lineLeft_ = lineLeft_ + 1;
			std::cout << "White line down left detected." << endl;
		}
		else if ((edges_.at<uchar>(edges_.rows - 1, g) == 255) && (g > half_cols)) {
			lineRight_ = lineRight_ + 1;
			std::cout << "White line down right detected." << endl;
		}
	}

}

void BackgroundSubtractor::blade_decision_maker() {

	if ((lineLeft_ == 2) && (lineRight_ != 2) && (isLeftDefined_ == false)) {
		leftContours_ = contours_;
		leftHierarchy_ = hierarchy_;
		leftContourIndex_ = largestContourIndex_;
		isLeftDefined_ = true;
		std::cout << "Saving parameters of left line."<< endl;
	}
	else if ((lineRight_ == 2) && (lineLeft_ != 2) && (isRightDefined_ == false)) {
		rightContours_ = contours_;
		rightHierarchy_ = hierarchy_;
		rightContourIndex_ = largestContourIndex_;
		isRightDefined_ = true;
		std::cout << "Saving parameters for right line."<< endl;
	}
	else if ((lineLeft_ == 2) && (lineRight_ == 2)) {
		leftContours_ = contours_;
		leftHierarchy_ = hierarchy_;
		leftContourIndex_ = largestContourIndex_;
		isLeftDefined_ = true;
		rightContours_ = contours_;
		rightHierarchy_ = hierarchy_;
		rightContourIndex_ = secondLargestContourIndex_;
		isRightDefined_ = true;
		twoGoodLines_ = true;
		isBladeDefined_ = true;
		std::cout << "Both parameters stored." << endl;
	}
	if ((isLeftDefined_ == true) && (isRightDefined_ == true))
		isBladeDefined_ = true;
}

void BackgroundSubtractor::blade_drawer() {
	std::cout << "Prepare drawing the two lines." << endl;
	Mat final_image = Mat::zeros(image_.size(), CV_8UC3);
	cv::drawContours(final_image, leftContours_, leftContourIndex_, color_, 1, 8, leftHierarchy_, 0, Point());
	cv::drawContours(final_image, rightContours_, rightContourIndex_, color_, 1, 8, rightHierarchy_, 0, Point());
	//imwrite("canvas.jpg", final_image);
	cv::cvtColor(final_image, final_image, CV_BGR2GRAY);
	cv::threshold(final_image, final_image, 220, 255, THRESH_BINARY);
	//cv::imwrite("canvas.jpg", final_image);


	subtractedBackground = Mat::zeros(image_.size(), CV_8UC3);
	std::cout << "Subtracting background... " << endl;

	if (isLeftDefined_ == true) {
		bool line_started = false;
		for (int i = 0; i < final_image.rows; i++) {
			line_started = false;
			int line_column = 0;
			for (int j = 0; j < final_image.cols; j++) {
				if ((final_image.at<uchar>(i, j) == 255) && (line_started == false)) {
					line_started = true;
					line_column = j;
				}
				else if ((final_image.at<uchar>(i, j) == 0) && (line_started == true)) {
					subtractedBackground.at<cv::Vec3b>(i, j) = image_.at<cv::Vec3b>(i, j);
				}
				else if ((final_image.at<uchar>(i, j) == 255) && (line_started == true) && (j > line_column + 100)) {
					break;
				}
			}
		}
	}
	else if ((isLeftDefined_ == false) && (isRightDefined_ == true)) {
		bool line_started = false;
		for (int i = 0; i < final_image.rows; i++) {
			line_started = false;
			int line_column = 0;
			for (int j = final_image.cols - 1; j >= 0; j--) {
				if ((final_image.at<uchar>(i, j) == 255) && (line_started == false)) {
					line_started = true;
					line_column = j;
				}
				else if ((final_image.at<uchar>(i, j) == 0) && (line_started == true)) {
					subtractedBackground.at<cv::Vec3b>(i, j) = image_.at<cv::Vec3b>(i, j);
				}
				else if ((final_image.at<uchar>(i, j) == 255) && (line_started == true) && (j < line_column - 100)) {
					break;
				}
			}
		}
	}
	else {
		std::cout << "No lines have been identified. Write something and press enter to exit." << endl;
		int b;
		std::cin >> b;
	}
}