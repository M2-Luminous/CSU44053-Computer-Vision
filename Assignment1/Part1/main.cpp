#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

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

Mat Threshold(const Mat& img) {
    Mat thresh;
    threshold(img, thresh, 150, 255, THRESH_BINARY);
    return thresh;
}

Mat ConvertHSV(const Mat& img) {
    Mat img_hsv;
    cvtColor(img, img_hsv, COLOR_BGR2HSV);
    return img_hsv;
}

int main() {
    string img_path = "../Assignment1/Balls/Ball10.jpg";

    // Read the specific image
    Mat img = imread(img_path);

    // Check if the image is loaded correctly
    if (img.empty()) {
        cerr << "Error: Image not loaded!" << endl;
        return -1;
    }

    // Converting to grayscale
    Mat img_gray = ConvertToGray(img);

    // Thresholding
    Mat thresh = Threshold(img_gray);

    vector<Vec3f> circles = DetectCircles(img_gray);

    if (!circles.empty()) {
        for (size_t i = 0; i < circles.size(); ++i) {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // draw the outer circle
            circle(img, center, radius, Scalar(0, 255, 0), 2);
            // draw the center of the circle
            circle(img, center, 2, Scalar(0, 0, 255), 3);
            // Print the center coordinates of the circle and the diameter
            cout << center.x << " " << center.y << " " << 2 * radius << endl;
        }
    }

    // Display the processed image
    imshow("detected circles", img);

    //string output_path = "../Assignment1/Balls/Ball10_detected.jpg";

    // Save the processed image
    //imwrite(output_path, img);

    // Wait for a key press
    waitKey(0);

    destroyAllWindows();

    return 0;
}
