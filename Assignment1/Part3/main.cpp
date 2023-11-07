#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main() {
    VideoCapture cap("../Assignment1_part3/TableTennis.avi");

    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open video file." << std::endl;
        return -1;
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

        for (const auto &contour : contours) {
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
        } else {
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
            } else {
                displayMessage = ""; // Clear message if no specific condition is met
            }

            if (currentPos.y < prevPos.y) {
                if (bounce_start_frame == -1) {
                    bounce_start_frame = 1; // Start checking for bounce
                }
            } else {
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
            } else {
                ball_left_hit = true;
            }

            if (currentPos.x > prevPos.x) {
                if (ball_right_hit) {
                    std::cout << "Frame " << currentFrame << ": Ball hit by the left player!" << std::endl;
                    displayMessage = "Ball hit by the left player!";
                    ball_right_hit = false;
                }
            } else {
                ball_right_hit = true;
            }

            prevPos = currentPos;
        }

        // Draw unfilled circles for all detected ball positions
        for (const auto &position : ballPositions) {
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

    return 0;
}
