#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include <string>

using namespace cv;
using namespace std;
// Include other necessary headers and namespaces here

// Function prototypes
void function1();
void function2();
void function3();

int main() {
    int choice;

    std::cout << "Enter 1, 2, or 3 to execute a functionality: ";
    std::cin >> choice;

    switch (choice) {
    case 1:
        function1();
        break;
    case 2:
        function2();
        break;
    case 3:
        function3();
        break;
    default:
        std::cout << "Invalid choice. Please enter 1, 2, or 3.\n";
    }

    return 0;
}

// Definitions for function1, function2, and function3
// Function prototypes
Mat ConvertToGray(const Mat& img);
vector<Vec3f> DetectCircles(const Mat& img);

void function1() {
    for (int i = 1; i <= 10; ++i) {
        string img_path = "../Assignment1/Balls/Ball" + to_string(i) + ".jpg";

        Mat img = imread(img_path);

        if (img.empty()) {
            cerr << "Error: Image " << img_path << " not loaded!" << endl;
            continue;
        }

        Mat img_gray = ConvertToGray(img);
        vector<Vec3f> circles = DetectCircles(img_gray);

        if (!circles.empty()) {
            for (size_t j = 0; j < circles.size(); ++j) {
                Point center(cvRound(circles[j][0]), cvRound(circles[j][1]));
                int radius = cvRound(circles[j][2]);
                circle(img, center, radius, Scalar(0, 255, 0), 2);
                circle(img, center, 2, Scalar(0, 0, 255), 3);
                cout << "Ball" << i << " - Center: (" << center.x << ", " << center.y << "), Diameter: " << 2 * radius << endl;
            }
        }

        imshow("detected circles - Ball" + to_string(i), img);

        int keyPressed = waitKey(0);

        destroyWindow("detected circles - Ball" + to_string(i));

        if (keyPressed == 27) {
            break;
        }
    }

    destroyAllWindows();
}

// Definitions of ConvertToGray and DetectCircles functions
Mat ConvertToGray(const Mat& img) {
    Mat img_gray;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);
    GaussianBlur(img_gray, img_gray, Size(13, 13), BORDER_DEFAULT);
    return img_gray;
}

vector<Vec3f> DetectCircles(const Mat& img) {
    vector<Vec3f> circles;
    HoughCircles(img, circles, HOUGH_GRADIENT, 1, 20, 50, 30, 15, 80);
    return circles;
}

// Function prototype for rescaleFrame
void rescaleFrame(Mat& frame, double scale);

// New function2 definition
void function2() {
    char* file_location1 = "../Assignment1/tables/";
    char* image_files1[] = {
        "Table1.jpg",
        "Table2.jpg",
        "Table3.jpg",
        "Table4.jpg",
        "Table5.jpg",
    };

    int number_of_images1 = sizeof(image_files1) / sizeof(image_files1[0]);
    Mat* image1 = new Mat[number_of_images1];
    for (int file_no = 0; (file_no < number_of_images1); file_no++) {
        string filename(file_location1);
        filename.append(image_files1[file_no]);
        image1[file_no] = imread(filename, -1);
        if (image1[file_no].empty()) {
            cout << "Could not open " << image1[file_no] << endl;
            return;
        }
    }

    for (int i = 0; i < number_of_images1; i++) {
        Mat current_image = image1[i].clone();
        if (current_image.empty()) {
            std::cout << "Could not open or find the image!" << std::endl;
            return;
        }
        Mat hsv;
        cvtColor(current_image, hsv, COLOR_BGR2HSV);
        vector<Mat> hsvChannels;
        split(hsv, hsvChannels);
        Mat v = hsvChannels[2]; // V channel
        Mat sat = hsvChannels[1]; // S channel

        // Apply Histogram Equalization to the V channel
        equalizeHist(v, v);

        // Use multiple iterations of CLAHE on the V channel
        Ptr<CLAHE> clahe = createCLAHE();
        clahe->setClipLimit(3);
        clahe->setTilesGridSize(Size(1, 1));

        int numIterations = 5;
        for (int j = 0; j < numIterations; ++j) {
            clahe->apply(v, v);
        }

        // Adjust the saturation
        float saturationMultiplier = 2.2;
        for (int y = 0; y < sat.rows; y++) {
            for (int x = 0; x < sat.cols; x++) {
                int newVal = sat.at<uchar>(y, x) * saturationMultiplier;
                sat.at<uchar>(y, x) = (newVal > 255) ? 255 : newVal;
            }
        }

        // Merge channels back into the HSV image
        hsvChannels[1] = sat;
        hsvChannels[2] = v;
        merge(hsvChannels, hsv);

        // Define the range for blue color in HSV
        Scalar lower_blue = Scalar(90, 50, 35);
        Scalar upper_blue = Scalar(170, 255, 255);

        // Threshold the HSV image to get only blue colors
        Mat blueMask;
        inRange(hsv, lower_blue, upper_blue, blueMask);

        // Dilate the blueMask
        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
        dilate(blueMask, blueMask, kernel, Point(-1, -1), 1);

        // Apply the blue color mask
        Mat blueFiltered;
        current_image.copyTo(blueFiltered, blueMask);

        // Find contours
        vector<vector<Point>> contours;
        findContours(blueMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // Iterate through each contour
        for (size_t j = 0; j < contours.size(); j++) {
            vector<Point> contour_poly;
            approxPolyDP(contours[j], contour_poly, 3, true);

            // Check if the polygon is a quadrilateral
            if (contour_poly.size() == 4) {
                for (size_t k = 0; k < 4; k++) {
                    circle(current_image, contour_poly[k], 5, Scalar(0, 255, 0), -1);
                }
            }
        }

        // Rescale the images for display
        rescaleFrame(current_image, 0.15);
        rescaleFrame(blueFiltered, 0.15);

        // Display the images
        imshow("Original Image", current_image);
        imshow("Blue Color Filtered Image", blueFiltered);

        // Wait for a key press
        waitKey(0);
        destroyAllWindows();
    }

    // Make sure to delete the dynamically allocated array
    delete[] image1;
}

// Definition of rescaleFrame function
void rescaleFrame(Mat& frame, double scale) {
    int width = frame.size().width * scale;
    int height = frame.size().height * scale;
    resize(frame, frame, Size(width, height), 0, 0, INTER_AREA);
}


void function3() {
    VideoCapture cap("../Assignment1/TableTennis.avi");

    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open video file." << std::endl;
        return;
    }

    Mat frame, hsv, mask, gray, diffGray;
    Scalar lowerBound(0, 100, 220);
    Scalar upperBound(20, 255, 255);
    std::vector<Point2f> ballPositions;
    Mat prevGray;
    Point2f prevPos;
    int frames_without_ball = 0;
    int bounce_start_frame = -1;
    bool ball_left_hit = false;
    bool ball_right_hit = false;

    double fps = cap.get(CAP_PROP_FPS); // Get the video's frame rate

    int currentFrame = 0; // Variable to hold the current frame number
    std::string displayMessage; // Variable to hold the message

    while (cap.read(frame)) {
        currentFrame++; // Increment frame number
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, lowerBound, upperBound, mask);

        cvtColor(frame, gray, COLOR_BGR2GRAY);

        if (!prevGray.empty()) {
            absdiff(gray, prevGray, diffGray);
            threshold(diffGray, diffGray, 10, 255, THRESH_BINARY); // Adjusted threshold value
            bitwise_and(mask, diffGray, mask);
        }

        std::vector<std::vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        ballPositions.clear(); // Clear previous positions

        for (const auto& contour : contours) {
            double area = contourArea(contour);
            if (area > 30) { // Adjusted minimum area threshold
                Moments mu = moments(contour, false);
                Point2f mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
                ballPositions.push_back(mc);
            }
        }

        if (ballPositions.empty()) {
            frames_without_ball++;
            if (frames_without_ball > 10) {
                std::cout << "Frame " << currentFrame << ": The ball is out of bounds!" << std::endl;
                displayMessage = "The ball is out of bounds!";
                frames_without_ball = 0;
                bounce_start_frame = -1;
                ball_left_hit = false;
                ball_right_hit = false;
            }
        }
        else {
            frames_without_ball = 0;

            Point2f currentPos = ballPositions.back();

            // Define the rectangle coordinates for the net detection
            Rect netRectangle(470, 370, 20, 100); // Adjust coordinates and size as needed
            rectangle(frame, netRectangle, Scalar(0, 0, 255), 2); // Draw the net rectangle

            int padding = 5; // Adjust the padding as needed
            if (currentPos.y >= netRectangle.y - padding && currentPos.y <= netRectangle.y + netRectangle.height + padding &&
                currentPos.x >= netRectangle.x - padding && currentPos.x <= netRectangle.x + netRectangle.width + padding) {
                std::cout << "Frame " << currentFrame << ": Ball hit the net boundary!" << std::endl;
                displayMessage = "Ball hit the net boundary!";
            }
            else {
                displayMessage = ""; // Clear message if no specific condition is met
            }

            if (currentPos.y < prevPos.y) {
                if (bounce_start_frame == -1) {
                    bounce_start_frame = 1; // Start checking for bounce
                }
            }
            else {
                if (bounce_start_frame > 0) {
                    bounce_start_frame++;
                }
            }

            if (bounce_start_frame > 1 && currentPos.y < prevPos.y) {
                std::cout << "Frame " << currentFrame << ": The ball bounced from the table!" << std::endl;
                displayMessage = "The ball bounced from the table!";
                bounce_start_frame = -1;
            }

            if (currentPos.x < prevPos.x) {
                if (ball_left_hit) {
                    std::cout << "Frame " << currentFrame << ": Ball hit by the right player!" << std::endl;
                    displayMessage = "Ball hit by the right player!";
                    ball_left_hit = false;
                }
            }
            else {
                ball_left_hit = true;
            }

            if (currentPos.x > prevPos.x) {
                if (ball_right_hit) {
                    std::cout << "Frame " << currentFrame << ": Ball hit by the left player!" << std::endl;
                    displayMessage = "Ball hit by the left player!";
                    ball_right_hit = false;
                }
            }
            else {
                ball_right_hit = true;
            }

            prevPos = currentPos;
        }

        // Draw unfilled circles for all detected ball positions
        for (const auto& position : ballPositions) {
            circle(frame, position, 10, Scalar(0, 255, 0), 2); // Use a thickness of 2 for the outline
        }

        // Display the stored message on the video frame
        putText(frame, displayMessage, Point(10, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        imshow("Table Tennis Video", frame);

        char c = waitKey(1000 / fps); // Delay to maintain original speed

        if (c == 27) { // ESC key to exit
            break;
        }

        prevGray = gray.clone(); // Redefine prevGray
    }

    cap.release();
    destroyAllWindows();
}
