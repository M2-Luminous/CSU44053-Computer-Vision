

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
        "Table2.jpg",
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
        Mat resizedImage;
        Mat current_image = image1[i].clone();
        cv::Mat hsv;
        cv::cvtColor(current_image, hsv, cv::COLOR_BGR2HSV);

        // Split the HSV image into individual channels
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
        int numIterations = 1; // Adjust the number of iterations as needed ! 
        for (int i = 0; i < numIterations; ++i) {
            clahe->apply(v, v);
        }

        // Adjust the saturation by multiplying the values
        float saturationMultiplier = 5.6; // Experiment with different values !
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
        cv::Mat processedImage;
        cv::cvtColor(hsv, processedImage, cv::COLOR_HSV2BGR);

        // Define the range for blue color in HSV
        // These values may need tweaking depending on the exact shade of blue of your table
        cv::Scalar lower_blue = cv::Scalar(90, 60, 70);  //!
        cv::Scalar upper_blue = cv::Scalar(120, 103, 110);  //!

        // Threshold the HSV image to get only blue colors
        cv::Mat blueMask;
        cv::inRange(processedImage, lower_blue, upper_blue, blueMask);

        // Dilate the blueMask if needed to emphasize the blue regions
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        cv::dilate(blueMask, blueMask, kernel, cv::Point(-1, -1), 1);

        // Rescale the images for display
        rescaleFrame(current_image, 0.15);
        rescaleFrame(blueMask, 0.15);

        Mat canny_image;
        Canny(blueMask, canny_image, 50, 200, 3);

        vector<Vec2f> lines; // will hold the results of the detection
        HoughLines(blueMask, lines, 1, CV_PI / 180, 50, 0, 0); // runs the actual detection
        // Draw the lines
        for (size_t i = 0; i < lines.size(); i++)
        {
            float rho = lines[i][0], theta = lines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            pt1.x = cvRound(x0 + 1000 * (-b));
            pt1.y = cvRound(y0 + 1000 * (a));
            pt2.x = cvRound(x0 - 1000 * (-b));
            pt2.y = cvRound(y0 - 1000 * (a));
            line(current_image, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
        }

        // Display the processed image after multiple CLAHE iterations and saturation adjustment
        imshow("Processed Image with Enhanced CLAHE and Saturation Adjustment", current_image);

        // Display the result image with the blue color filtered
        imshow("Blue Color Filtered Image", blueMask);

        // Wait for a key press and close the windows
        cv::waitKey(0);
        cv::destroyAllWindows();

        return 0;
    }

}

