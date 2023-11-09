

/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe   Wiley & Sons Inc. 2014.  All rights reserved.
 */
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <fstream>
#include <list>
#include <chrono>
#include <thread>

using namespace cv;
using namespace std;

void rescaleFrame(cv::Mat& frame, double scale) {
    int width = frame.size().width * scale;
    int height = frame.size().height * scale;
    cv::resize(frame, frame, cv::Size(width, height), 0, 0, cv::INTER_AREA);
}


int main(int argc, const char** argv)
{

    char* file_location1 = "../Assignment_part2/tables/";
    char* image_files1[] = {
        "Table1.jpg",
        "Table2.jpg",
        "Table3.jpg",
        "Table4.jpg",
        "Table5.jpg",
    };

    int number_of_images1 = sizeof(image_files1) / sizeof(image_files1[0]);
    Mat* image1 = new Mat[number_of_images1];
    for (int file_no = 0; (file_no < number_of_images1); file_no++)
    {
        string filename(file_location1);
        filename.append(image_files1[file_no]);
        image1[file_no] = imread(filename, -1);
        if (image1[file_no].empty())
        {
            cout << "Could not open " << image1[file_no] << endl;
            return -1;
        }
    }

    for (int i = 0; i < number_of_images1; i++) {
        Mat current_image = image1[i].clone();
        if (current_image.empty()) {
            std::cout << "Could not open or find the image!" << std::endl;
            return -1;
        }
        cv::Mat hsv;
        cv::cvtColor(current_image, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> hsvChannels;
        cv::split(hsv, hsvChannels);
        cv::Mat v = hsvChannels[2]; // V channel
        cv::Mat sat = hsvChannels[1]; // S channel

        // Apply Histogram Equalization to the V channel
        cv::equalizeHist(v, v);

        // Use multiple iterations of CLAHE on the V channel to reduce light reflection
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(3); // You can adjust this value
        clahe->setTilesGridSize(cv::Size(1, 1)); // You can adjust grid size !

        // Perform multiple iterations of CLAHE
        int numIterations = 5; // Adjust the number of iterations as needed ! 
        for (int i = 0; i < numIterations; ++i) {
            clahe->apply(v, v);
        }

        // Adjust the saturation by multiplying the values
        float saturationMultiplier = 2.2; // Experiment with different values !
        for (int j = 0; j < sat.rows; j++) {
            for (int i = 0; i < sat.cols; i++) {
                int newVal = sat.at<uchar>(j, i) * saturationMultiplier;
                sat.at<uchar>(j, i) = (newVal > 255) ? 255 : newVal;
            }
        }

        // Merge the adjusted channels back into the HSV image
        hsvChannels[1] = sat;
        hsvChannels[2] = v;
        cv::merge(hsvChannels, hsv);

        // Convert the modified HSV image back to BGR
        //cv::Mat processedImage;
        //cv::cvtColor(hsv, processedImage, cv::COLOR_HSV2BGR);

        // Convert the image to HSV
        //cv::Mat hsv;
        //cv::cvtColor(current_image, hsv, cv::COLOR_BGR2HSV);

        // Define the range for blue color in HSV
        cv::Scalar lower_blue = cv::Scalar(90, 50, 50); // Lower bound for blue color in HSV
        cv::Scalar upper_blue = cv::Scalar(170, 255, 255); // Upper bound for blue color in HSV

        // Threshold the HSV image to get only blue colors
        cv::Mat blueMask;
        cv::inRange(hsv, lower_blue, upper_blue, blueMask);

        // Dilate the blueMask if needed to emphasize the blue regions
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        cv::dilate(blueMask, blueMask, kernel, cv::Point(-1, -1), 1);

        // Apply the blue color mask to the original image
        cv::Mat blueFiltered;
        current_image.copyTo(blueFiltered, blueMask);

        // Rescale the images for display
        rescaleFrame(current_image, 0.15);
        rescaleFrame(blueFiltered, 0.15);

        // Display the original image
        cv::imshow("Original Image", current_image);

        // Display the result image with the blue color filtered
        cv::imshow("Blue Color Filtered Image", blueFiltered);

        // Wait for a key press and close the windows
        cv::waitKey(0);
        cv::destroyAllWindows();
    }

}

