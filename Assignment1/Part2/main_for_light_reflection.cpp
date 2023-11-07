

/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe   Wiley & Sons Inc. 2014.  All rights reserved.
 */
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void rescaleFrame(cv::Mat& frame, double scale) {
    int width = frame.size().width * scale;
    int height = frame.size().height * scale;
    cv::resize(frame, frame, cv::Size(width, height), 0, 0, cv::INTER_AREA);
}

bool computeIntersection(Vec2f line1, Vec2f line2, Point2f& intersection) {
    float rho1 = line1[0], theta1 = line1[1];
    float rho2 = line2[0], theta2 = line2[1];

    double a1 = cos(theta1), b1 = sin(theta1);
    double a2 = cos(theta2), b2 = sin(theta2);

    double det = a1 * b2 - a2 * b1;
    if (fabs(det) < 1e-6) { // Lines are parallel or coincident
        return false;
    }

    double x = (b2 * rho1 - b1 * rho2) / det;
    double y = (a1 * rho2 - a2 * rho1) / det;

    intersection = Point2f(x, y);
    return true; // Intersection point found
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

        // Detect lines using Hough Line Transform
        vector<Vec2f> lines;
        HoughLines(blueMask, lines, 1, CV_PI / 180, 50, 0, 0); // The '50' here is the threshold

        // Merge closely overlapping lines
        const float minDistance = 30; // Set the minimum distance to consider lines as similar
        const float minAngle = CV_PI / 18; // Set the minimum angle difference to consider lines as similar (10 degrees)

        vector<Vec2f> mergedLines;
        for (size_t i = 0; i < lines.size(); ++i) {
            float rho1 = lines[i][0], theta1 = lines[i][1];
            bool merged = false;

            for (size_t j = 0; j < mergedLines.size(); ++j) {
                float rho2 = mergedLines[j][0], theta2 = mergedLines[j][1];

                // Check if lines are similar (close and almost parallel)
                if (fabs(rho1 - rho2) < minDistance && fabs(theta1 - theta2) < minAngle) {
                    // Merge lines by averaging their parameters
                    mergedLines[j] = Vec2f((rho1 + rho2) / 2, (theta1 + theta2) / 2);
                    merged = true;
                    break;
                }
            }

            if (!merged) {
                mergedLines.push_back(lines[i]);
            }
        }

        // Proceed with drawing merged lines
        for (size_t i = 0; i < mergedLines.size(); ++i) {
            float rho = mergedLines[i][0], theta = mergedLines[i][1];

            // Convert polar coordinates to endpoints
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
            Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));

            // Draw the merged lines
            line(current_image, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
        }

        // Make a copy of the original image for displaying red dots
        Mat redDotsImage = image1[i].clone();
        rescaleFrame(redDotsImage, 0.15);

        // Find intersections among merged lines
        vector<Point2f> intersections;

        for (size_t i = 1; i < mergedLines.size(); ++i) {
            for (size_t j = i + 1; j < mergedLines.size(); ++j) {
                float rho1 = mergedLines[i][0], theta1 = mergedLines[i][1];
                float rho2 = mergedLines[j][0], theta2 = mergedLines[j][1];

                // Calculate intersection between merged lines
                Point2f intersectionPoint;
                if (computeIntersection(Vec2f(rho1, theta1), Vec2f(rho2, theta2), intersectionPoint)) {
                    intersections.push_back(intersectionPoint);
                }
            }
        }

        // Draw red dots at intersection points on the redDotsImage
        for (const auto& intersection : intersections) {
            circle(redDotsImage, intersection, 5, Scalar(0, 0, 255), -1);
        }


        // Display the updated image with red dots representing intersections on the original image
        imshow("Red Dots at Intersections", redDotsImage);

        // Display the result image with the blue color filtered
        imshow("Blue Color Filtered Image", blueMask);

        // Display the processed image after multiple CLAHE iterations and saturation adjustment
        imshow("Processed Image with Enhanced CLAHE and Saturation Adjustment", current_image);



        // Wait for a key press and close the windows
        cv::waitKey(0);
        cv::destroyAllWindows();

        return 0;
    }

}

