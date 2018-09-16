//Portions of code courtesy of George Lecakes were used in the development of this program
//Available from https://www.youtube.com/playlist?list=PLAp0ZhYvW6XbEveYeefGSuLhaPlFML9gP
#include "opencv2\opencv.hpp"
#include <stdint.h>
#include <cmath>
#include <iostream>

using namespace cv;

//Demo of creation, read, display & save image
void demoBasic()
{
	//Create image object
	Mat test = imread("PandaOriginal.bmp", CV_LOAD_IMAGE_UNCHANGED);
	//Display image on screen
	imshow("Panda Original", test);
	//Keep image on screen until key is pressed
	waitKey();
	//Save image
	imwrite("outputPanda.bmp", test);
}

//Demo of manipulating image windows
void demoWindows()
{
	Mat file1 = imread("PandaOriginal.bmp", CV_LOAD_IMAGE_UNCHANGED);

	//Different window types avialable
	namedWindow("resize", CV_WINDOW_FREERATIO);
	namedWindow("fixed", CV_WINDOW_AUTOSIZE);

	//Show window types
	imshow("resize", file1);
	imshow("fixed", file1);

	//Resize windows, FREERATION allows this, AUTOSIZE will cut off image
	//resizeWindow("resize", file1.cols/2, file1.rows/2);
	//resizeWindow("fixed", file1.cols/2, file1.rows/2);

	//Move image around screen
	moveWindow("resize", 800 + file1.cols, 500);
	moveWindow("fixed", 800, 500);

	waitKey();
}

//Demo of manipulating individual pixels
void demoPixel()
{
	Mat original = imread("PandaOriginal.bmp", CV_LOAD_IMAGE_UNCHANGED);
	Mat modified = imread("PandaOriginal.bmp", CV_LOAD_IMAGE_UNCHANGED);

	//Double for loop to access all pixels (2D Arrary)
	for (int r = 0; r < modified.rows; r++)
	{
		for (int c = 0; c < modified.cols; c++)
		{
			//unint8_t = 0-255 values
			modified.at<uint8_t>(r, c) = original.at<uint8_t>(r, c) * 0.5f;
		}
	}

	imshow("Original", original);
	imshow("Modified", modified);
	waitKey();
}

//Complete prep work for and then make DFT
void makeDFT(Mat& source, Mat& destination)
{
	//Requires two channels, real and imaginary
	//so create array of Mats which can be merged later
	Mat originalComplex[2] = { source, Mat::zeros(source.size(), CV_32F) };

	//matrix ready for dft
	Mat dftReady;

	//Merge channels
	merge(originalComplex, 2, dftReady);

	//Make DFT
	Mat dftOfOriginal;
	dft(dftReady, dftOfOriginal, DFT_COMPLEX_OUTPUT);

	destination = dftOfOriginal;
}

void recentreDFT(Mat& source)
{
	//Original -> Recentered
	//1    2        4    3
	//3    4        2    1
	int centreX = source.cols / 2;
	int centreY = source.rows / 2;

	Mat q1(source, Rect(0, 0, centreX, centreY));
	Mat q2(source, Rect(centreX, 0, centreX, centreY));
	Mat q3(source, Rect(0, centreY, centreX, centreY));
	Mat q4(source, Rect(centreX, centreY, centreX, centreY));

	Mat tempSwap;

	q1.copyTo(tempSwap);
	q4.copyTo(q1);
	tempSwap.copyTo(q4);

	q2.copyTo(tempSwap);
	q3.copyTo(q2);
	tempSwap.copyTo(q3);
}

void showDFT(Mat& source, String savename)
{
	//split dft into two channels
	Mat splitArray[2] = { Mat::zeros(source.size(), CV_32F), Mat::zeros(source.size(), CV_32F) };
	split(source, splitArray);

	Mat dftMagnitude;

	magnitude(splitArray[0], splitArray[1], dftMagnitude);

	dftMagnitude += Scalar::all(1);

	log(dftMagnitude, dftMagnitude);

	normalize(dftMagnitude, dftMagnitude, 0, 1, CV_MINMAX);

	recentreDFT(dftMagnitude);

	imshow("DFT", dftMagnitude);
	dftMagnitude.convertTo(dftMagnitude, CV_8UC3, 255.0);
	imwrite(savename + ".jpg", dftMagnitude);

	waitKey();
}

void invertDFT(Mat& source, Mat& destination)
{
	Mat inverse;

	dft(source, inverse, DFT_INVERSE | DFT_REAL_OUTPUT | DFT_SCALE);

	destination = inverse;
}

void standardFilters(Mat& source, String filePrefix)
{
	Mat gaussianBlurImg;
	Mat medianBlurImg;
	Mat bilateralFilterImg;

	GaussianBlur(source, gaussianBlurImg, Size(5, 5), 100, 100);
	imshow("gaussianBlur", gaussianBlurImg);
	imwrite(filePrefix + "GaussianBlur.jpg", gaussianBlurImg);
	medianBlur(source, medianBlurImg, 5);
	imshow("medianBlur", medianBlurImg);
	imwrite(filePrefix + "MedianBlur.jpg", medianBlurImg);
	bilateralFilter(source, bilateralFilterImg, 7, 150, 150, BORDER_DEFAULT);
	imshow("bilateralFilter", bilateralFilterImg);
	imwrite(filePrefix + "BilateralFilter.jpg", bilateralFilterImg);

	Mat float1;
	gaussianBlurImg.convertTo(float1, CV_32FC1, 1.0 / 255.0);
	Mat dft1;
	makeDFT(float1, dft1);
	showDFT(dft1, filePrefix + "GaussianDFT");

	Mat float2;
	medianBlurImg.convertTo(float2, CV_32FC1, 1.0 / 255.0);
	Mat dft2;
	makeDFT(float2, dft2);
	showDFT(dft2, filePrefix + "MedianDFT");

	Mat float3;
	bilateralFilterImg.convertTo(float3, CV_32FC1, 1.0 / 255.0);
	Mat dft3;
	makeDFT(float3, dft3);
	showDFT(dft3, filePrefix + "BilateralDFT");
}

int getAverage(Mat source, int r, int c)
{
	//std::cout << r << ", " << c << "\n";
	int total;
	
	if (r == 0 && c == 0) {
		total = source.at<uint8_t>(r + 1, c) +
			source.at<uint8_t>(r + 1, c + 1) +
			source.at<uint8_t>(r, c + 1);
		if (total > 0) { return (int)(total / 3); }
		return 0;
	}
	else if (r == source.rows - 1 && c == source.cols - 1) {
		total = source.at<uint8_t>(r - 1, c) +
				source.at<uint8_t>(r - 1, c - 1) +
				source.at<uint8_t>(r, c - 1);
		if (total > 0) { return (int)(total / 3); }
		return 0;
	}
	else if (r == 0) {
		total = source.at<uint8_t>(r + 1, c + 1) +
				source.at<uint8_t>(r + 1, c) +
				source.at<uint8_t>(r + 1, c - 1) +
				source.at<uint8_t>(r, c + 1) +
				source.at<uint8_t>(r, c - 1);
		if (total > 0) { return (int)(total / 5); }
		return 0;
	}
	else if (c == 0) {
		total = source.at<uint8_t>(r + 1, c + 1) +
				source.at<uint8_t>(r + 1, c) +
				source.at<uint8_t>(r - 1, c + 1) +
				source.at<uint8_t>(r - 1, c) +
				source.at<uint8_t>(r, c + 1);
		if (total > 0) { return (int)(total / 5); }
		return 0;
	}
	else if (r == source.rows - 1) {
		total = source.at<uint8_t>(r - 1, c + 1) +
				source.at<uint8_t>(r - 1, c) +
				source.at<uint8_t>(r - 1, c - 1) +
				source.at<uint8_t>(r, c + 1) +
				source.at<uint8_t>(r, c - 1);
		if (total > 0) { return (int)(total / 5); }
		return 0;
	}
	else if (c == source.cols - 1) {
		total = source.at<uint8_t>(r + 1, c - 1) +
				source.at<uint8_t>(r + 1, c) +
				source.at<uint8_t>(r - 1, c - 1) +
				source.at<uint8_t>(r - 1, c) +
				source.at<uint8_t>(r, c - 1);
		if (total > 0) { return (int)(total / 5); }
		return 0;
	}
	else {
		total =
		source.at<uint8_t>(r + 1, c + 1) +
		source.at<uint8_t>(r + 1, c) +
		source.at<uint8_t>(r + 1, c - 1) +
		source.at<uint8_t>(r - 1, c + 1) +
		source.at<uint8_t>(r - 1, c) +
		source.at<uint8_t>(r - 1, c - 1) +
		source.at<uint8_t>(r, c + 1) +
		source.at<uint8_t>(r, c - 1);
		if (total > 0) { return (int)(total / 8); }
		return 0;
	}

}

void pengRandomNoiseRemoval(Mat& source, Mat& dest, int min, int max)
{
	for (int r = 0; r < source.rows; r++)
	{

		for (int c = 0; c < source.cols; c++)
		{
			//std::cout << r << ", " << c << "\n";
			if (source.at<uint8_t>(r, c) < min)
				dest.at<uint8_t>(r, c) = (int)getAverage(source, r, c);
			if (source.at<uint8_t>(r, c) > max)
				dest.at<uint8_t>(r, c) = (int)getAverage(source, r, c);
		}
	}
}

int main(int argv, char** argc)
{
	Mat pandaNoise = imread("PandaNoise.bmp", CV_LOAD_IMAGE_UNCHANGED);
	Mat pandaOriginal = imread("PandaOriginal.bmp", CV_LOAD_IMAGE_UNCHANGED);

	Mat penguinNoise = imread("PenguinNoise.bmp", CV_LOAD_IMAGE_UNCHANGED);
	Mat penguinOriginal = imread("PenguinOriginal.bmp", CV_LOAD_IMAGE_UNCHANGED);

	Mat penguinOutput = imread("PenguinNoise.bmp", CV_LOAD_IMAGE_UNCHANGED);;

	standardFilters(pandaNoise, "panda");

	pengRandomNoiseRemoval(penguinNoise, penguinOutput, 50, 185);

	imshow("PenguinNoiseFilter", penguinOutput);
	imwrite("penguinNoiseFilter.jpg", penguinOutput);

	standardFilters(penguinOutput, "penguin");

	waitKey();
}