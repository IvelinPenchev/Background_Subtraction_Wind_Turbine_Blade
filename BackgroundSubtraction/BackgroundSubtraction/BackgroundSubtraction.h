#pragma once

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
	array<int, 6> median_{ 101, 101, 101, 201, 201, 201 };
	array<int, 6> brightness_{ 100, 50, -150, 100, 50, -150 };
	array<double, 6> contrast_{ 1, 1.8, 1.8, 1, 1.8, 1.8 };
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
