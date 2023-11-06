#py -m pip install opencv-contrib-python
import cv2
import numpy as np
from itertools import combinations

def rescale_frame(frame, scale):
    width = int(frame.shape[1] * scale)
    height = int(frame.shape[0] * scale)
    dimensions = (width, height)
    return cv2.resize(frame, dimensions, interpolation=cv2.INTER_AREA)

def find_largest_rectangle_contour(contours):
    max_area = 0
    largest_rectangle_contour = None

    for cnt in contours:
        approx = cv2.approxPolyDP(cnt, 0.01 * cv2.arcLength(cnt, True), True)
        if len(approx) == 4:
            area = cv2.contourArea(cnt)
            if area > max_area:
                max_area = area
                largest_rectangle_contour = cnt

    return largest_rectangle_contour

def compute_area(corner_set):
    return cv2.contourArea(np.array(corner_set))

# Load the image
image = cv2.imread("C:/Vision/Assignment_part2/tables/Table1.jpg")

# Convert the image to HSV
hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
v_channel = hsv[:, :, 2]
saturation = hsv[:, :, 1]

# Apply Histogram Equalization to the V channel
v_channel = cv2.equalizeHist(v_channel)

# Use CLAHE to further enhance details in the V channel
clahe = cv2.createCLAHE(clipLimit=3.0, tileGridSize=(1, 1))
for _ in range(5):
    v_channel = clahe.apply(v_channel)

# Adjust the saturation
saturation_multiplier = 2.0 
saturation = np.where((saturation * saturation_multiplier) > 255, 255, saturation * saturation_multiplier).astype(np.uint8)

# Merge the adjusted channels back into the HSV image
hsv[:, :, 2] = v_channel
hsv[:, :, 1] = saturation

# Define the range for blue color in HSV
lower_blue = np.array([90, 50, 35])#for table1
#lower_blue = np.array([90, 50, 45])#for table3, 4, 5
upper_blue = np.array([170, 255, 255])

# Threshold the HSV image to get only blue colors
mask = cv2.inRange(hsv, lower_blue, upper_blue)

# Dilate the mask to emphasize blue regions
kernel = np.ones((5, 5), np.uint8)
mask = cv2.dilate(mask, kernel, iterations=1)

# Apply the mask to the original image
blue_filtered = cv2.bitwise_and(image, image, mask=mask)

# Rescale the images for display
rescaled_image = rescale_frame(image, 0.15)
rescaled_blue_filtered = rescale_frame(blue_filtered, 0.15)

# Convert the blue-filtered image to gray
gray = cv2.cvtColor(blue_filtered, cv2.COLOR_BGR2GRAY)
gray = cv2.dilate(gray, kernel, iterations=10)
rescaled_gray = rescale_frame(gray, 0.15)

cv2.imshow("Grayscale Image", rescaled_gray)

# Find contours in the blue-filtered image
_, thresh = cv2.threshold(gray, 50, 255, 0)
contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
print("Number of contours detected:", len(contours))

# Find the largest rectangle contour
largest_rectangle_contour = find_largest_rectangle_contour(contours)

# Draw the largest rectangle on the blank image
blank_image = np.zeros((image.shape[0], image.shape[1], 3), dtype=np.uint8)
if largest_rectangle_contour is not None:
    cv2.drawContours(blank_image, [largest_rectangle_contour], -1, (0, 255, 0), 3)

# Detect corners
corners = cv2.goodFeaturesToTrack(cv2.cvtColor(blank_image, cv2.COLOR_BGR2GRAY), 100, 0.01, 10)
corners = np.int0(corners)

# Find the combination of four corners that form the largest area
max_area = 0
best_combination = []
for comb in combinations(corners, 4):
    area = compute_area(comb)
    if area > max_area:
        max_area = area
        best_combination = comb

# Draw the selected corners
for corner in best_combination:
    x, y = corner.ravel()
    cv2.circle(image, (x, y), 20, (0, 0, 255), -1)

cv2.imshow("Blue Color Filtered Image", rescaled_blue_filtered)
cv2.imshow("Shapes with Corners", rescale_frame(image, 0.25))
cv2.waitKey(0)
cv2.destroyAllWindows()
