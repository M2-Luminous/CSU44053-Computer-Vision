#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

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

int main() {
    for (int i = 1; i <= 10; ++i) {
        string img_path = "../Assignment1/Balls/Ball" + to_string(i) + ".jpg";

        Mat img = imread(img_path);

        if (img.empty()) {
            cerr << "Error: Image " << img_path << " not loaded!" << endl;
            continue;
        }

        Mat img_gray = ConvertToGray(img);
        Mat thresh = Threshold(img_gray);
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

    return 0;
}
