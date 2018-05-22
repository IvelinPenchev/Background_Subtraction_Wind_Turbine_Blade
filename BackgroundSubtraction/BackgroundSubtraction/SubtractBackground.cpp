#include "stdafx.h"
#include "opencv2\highgui\highgui.hpp"
#include "opencv\cv.h"
#include "opencv2/core/core.hpp"
#include "BackgroundSubtraction.h"

using namespace cv;

int main(int argc, char const *argv[]){
	const cv::String destination = "Small blade//6.JPG";
	BackgroundSubtractor subtractor;
	subtractor.background_subtraction(destination);
}