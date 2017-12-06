// SOULEVERDESDARONNES.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"


#include <windows.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include <string>  
#include <algorithm> // remove(), erase() 
#include <iostream>  
#include<sstream>
#include <filesystem>
#include <regex>
#include "opencv2\text.hpp"
#include "dirent.h"
#include <stdio.h>
#include <sys/types.h>

using namespace cv;
using namespace std;

cv::Mat matGrayscale;           //
cv::Mat matBlurred;             // declare more image variables
cv::Mat matThresh;              //
cv::Mat matThreshCopy;
Mat matTestingNumbers;
Mat frame;//
Mat heure;
int compteur;

// global variables ///////////////////////////////////////////////////////////////////////////////
const int MIN_CONTOUR_AREA = 100;

const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;

///////////////////////////////////////////////////////////////////////////////////////////////////






///////////////////////////////////////////////////////////////////////////////////////////////////
class ContourWithData {
public:
	// member variables ///////////////////////////////////////////////////////////////////////////
	std::vector<cv::Point> ptContour;           // contour
	cv::Rect boundingRect;                      // bounding rect for contour
	float fltArea;                              // area of contour

												///////////////////////////////////////////////////////////////////////////////////////////////
	bool checkIfContourIsValid() {                              // obviously in a production grade program
		if (fltArea < MIN_CONTOUR_AREA) return false;           // we would have a much more robust function for 
		return true;                                            // identifying if a contour is valid !!
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	static bool sortByBoundingRectXPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight) {      // this function allows us to sort
		return(cwdLeft.boundingRect.x < cwdRight.boundingRect.x);                                                   // the contours from left to right
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////



/** @function main */
int main()
{



	///////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<ContourWithData> allContoursWithData;           // declare empty vectors,
	std::vector<ContourWithData> validContoursWithData;         // we will fill these shortly

																// read in training classifications ///////////////////////////////////////////////////

	cv::Mat matClassificationInts;      // we will read the classification numbers into this variable as though it is a vector

	cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::READ);        // open the classifications file

	if (fsClassifications.isOpened() == false) {                                                    // if the file was not opened successfully
		std::cout << "error, unable to open training classifications file, exiting program\n\n";    // show error message
		return(0);                                                                                  // and exit program
	}

	fsClassifications["classifications"] >> matClassificationInts;      // read classifications section into Mat classifications variable
	fsClassifications.release();                                        // close the classifications file

																		// read in training images ////////////////////////////////////////////////////////////

	cv::Mat matTrainingImagesAsFlattenedFloats;         // we will read multiple images into this single image variable as though it is a vector

	cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::READ);          // open the training images file

	if (fsTrainingImages.isOpened() == false) {                                                 // if the file was not opened successfully
		std::cout << "error, unable to open training images file, exiting program\n\n";         // show error message
		return(0);                                                                              // and exit program
	}

	fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;           // read images section into Mat training images variable
	fsTrainingImages.release();                                                 // close the traning images file

																				// train //////////////////////////////////////////////////////////////////////////////

	cv::Ptr<cv::ml::KNearest>  kNearest(cv::ml::KNearest::create());            // instantiate the KNN object

																				// finally we get to the call to train, note that both parameters have to be of type Mat (a single Mat)
																				// even though in reality they are multiple images / numbers
	kNearest->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassificationInts);

	// test ///////////////////////////////////////////////////////////////////////////////





	int i = 0;
	struct dirent *lecture;
	DIR *rep;


	rep = opendir("D:/PROJET BACK IN THE ISSR/HMS-recordings");


	while ((lecture = readdir(rep))) {

		string str = lecture->d_name;	// nom du recording
		string str2 = "_170504_1540_";		// Date et heure detectee
		if (str.find(str2) != std::string::npos)
		{
			cout << str << endl;
		} // affiche les fichier qui match
		i++;
	}


	closedir(rep);



	VideoCapture cap("A.mp4"); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	cout << "cols : " << cap.get(CAP_PROP_FRAME_COUNT) << endl;
	for (;;)
	{
		compteur++;
		cap >> frame; // get a new frame from camera
		imshow("Image de base", frame);
		cout << "Width : " << frame.rows << endl;
		cout << "cols : " << frame.cols << endl;
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		namedWindow("Example1");
		cap.set(CAP_PROP_FPS, 10); //not changing anything
		std::cout << "\n\n" << "FPS = " << cap.get(CAP_PROP_FPS) << "\n\n";
		Mat heure = frame(Rect(1205, 695, 47, 20));
		Size size(150, 150);//the dst image size,e.g.100x100
		Mat agrandi;//src imag
		resize(heure, agrandi, size);//resize image
		Mat matTestingNumbers = agrandi;
		imshow("Example1", agrandi);

		cv::cvtColor(matTestingNumbers, matGrayscale, CV_BGR2GRAY);         // convert to grayscale
		cv::GaussianBlur(matGrayscale,              // input image
			matBlurred,                // output image
			cv::Size(1, 1),            // smoothing window width and height in pixels
			1);                       // sigma value, determines how much the image will be blurred, zero makes function choose the sigma value


									  // filter image from grayscale to black and white
		cv::adaptiveThreshold(matBlurred,                           // input image
			matThresh,                            // output image
			255,                                  // make pixels that pass the threshold full white
			cv::ADAPTIVE_THRESH_GAUSSIAN_C,       // use gaussian rather than mean, seems to give better results
			cv::THRESH_BINARY,                // invert so foreground will be white, background will be black
			9,                                   // size of a pixel neighborhood used to calculate threshold value
			0);                                   // constant subtracted from the mean or weighted mean

		imshow("Exaéémple1", matThresh);

		matThreshCopy = matThresh.clone();              // make a copy of the thresh image, this in necessary b/c findContours modifies the image

		std::vector<std::vector<cv::Point> > ptContours;        // declare a vector for the contours
		std::vector<cv::Vec4i> v4iHierarchy;                    // declare a vector for the hierarchy (we won't use this in this program but this may be helpful for reference)

		cv::findContours(matThreshCopy,             // input image, make sure to use a copy since the function will modify this image in the course of finding contours
			ptContours,                             // output contours
			v4iHierarchy,                           // output hierarchy
			cv::RETR_LIST,                      // retrieve the outermost contours only
			cv::CHAIN_APPROX_TC89_KCOS);               // compress horizontal, vertical, and diagonal segments and leave only their end points

		for (int i = 0; i < ptContours.size(); i++) {               // for each contour
			ContourWithData contourWithData;                                                    // instantiate a contour with data object
			contourWithData.ptContour = ptContours[i];                                          // assign contour to contour with data
			contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);         // get the bounding rect
			contourWithData.fltArea = cv::contourArea(contourWithData.ptContour);               // calculate the contour area
			allContoursWithData.push_back(contourWithData);                                     // add contour with data object to list of all contours with data
		}

		for (int i = 0; i < allContoursWithData.size(); i++) {                      // for all contours
			if (allContoursWithData[i].checkIfContourIsValid()) {                   // check if valid
				validContoursWithData.push_back(allContoursWithData[i]);            // if so, append to valid contour list
			}
		}
		// sort contours from left to right
		std::sort(validContoursWithData.begin(), validContoursWithData.end(), ContourWithData::sortByBoundingRectXPosition);

		std::string strFinalString;         // declare final string, this will have the final number sequence by the end of the program

		for (int i = 0; i < validContoursWithData.size(); i++)
		{            // for each contour

					 // draw a green rect around the current char
			cv::rectangle(matTestingNumbers,                            // draw rectangle on original image
				validContoursWithData[i].boundingRect,        // rect to draw
				cv::Scalar(0, 255, 0),                        // green
				2);                                           // thickness

			cv::Mat matROI = matThresh(validContoursWithData[i].boundingRect);          // get ROI image of bounding rect

			cv::Mat matROIResized;
			cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // resize image, this will be more consistent for recognition and storage

			cv::Mat matROIFloat;
			matROIResized.convertTo(matROIFloat, CV_32FC1);             // convert Mat to float, necessary for call to find_nearest

			cv::Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);

			cv::Mat matCurrentChar(0, 0, CV_32F);

			kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);     // finally we can call find_nearest !!!

			float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);

			strFinalString = strFinalString + char(int(fltCurrentChar));        // append current char to full string
		}

		string Str;
		Str = strFinalString;
		char Tableau[23] = { 'B','C','D','E','F','G','H','I','J','K','L','N','O','Q','R','S','T','U','V','W','X','Y','Z' };

		for (int i = 0; i < 24; i++)
		{
			Str.erase(
				std::remove(Str.begin(), Str.end(), Tableau[i]),
				Str.end());
		}
		std::cout << "\n\n" << "numbers read = " << strFinalString << "\n\n";
		std::cout << "\n\n" << "numbers read  clear= " << Str << "\n\n";
		// show the full string

		cv::imshow("matTestingNumbers", matTestingNumbers);     // show input image with green boxes drawn around found digits
		Str.clear(); // clean des data
		strFinalString.clear();
		validContoursWithData.clear();
		ptContours.clear();
		allContoursWithData.clear();//

		std::cout << "numero image= " << compteur << endl;
		// show the full string


		waitKey(5);
	}












	return 0; // yea, c++, no cleanup nessecary!ctor

}












