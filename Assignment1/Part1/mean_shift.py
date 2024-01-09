import cv2 as cv
import numpy as np
import os
import glob

if __name__ == '__main__':
    img_dir = "C:/Users/M2-Winterfell/Downloads/Computer-Vision-main/Computer Vision Assignment/Part_I/Balls"
    img_files = glob.glob(os.path.join(img_dir, 'Ball*.jpg'))

    for img_file in img_files:
        img_path = os.path.join(img_dir, img_file)
        img = cv.imread(img_path)

        # Applying Mean Shift Segmentation
        segmented_img = cv.pyrMeanShiftFiltering(img, sp=10, sr=100)

        # Converting to grayscale and blurring
        img_gray = cv.cvtColor(segmented_img, cv.COLOR_BGR2GRAY)
        img_gray = cv.GaussianBlur(img_gray, (13, 13), cv.BORDER_DEFAULT)

        # Detecting circles
        circles = cv.HoughCircles(img_gray, cv.HOUGH_GRADIENT, 1, 20, param1=50, param2=30, minRadius=15, maxRadius=100)

        # Drawing red circles on detected objects
        if circles is not None:
            circles_rounded = np.uint16(np.around(circles))
            for circle in circles_rounded[0, :]:
                cv.circle(img, (circle[0], circle[1]), circle[2], (0, 0, 255), 2)

        cv.imshow('Balls Detected', img)
        key = cv.waitKey(0)
        if key == ord('q'):
            break

    cv.destroyAllWindows()
