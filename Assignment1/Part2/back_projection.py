import cv2
import numpy as np

def rescale_image(image, scale):
    width = int(image.shape[1] * scale)
    height = int(image.shape[0] * scale)
    dimensions = (width, height)
    return cv2.resize(image, dimensions, interpolation=cv2.INTER_AREA)

def calculate_histogram(image):
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    hist = cv2.calcHist([hsv], [0, 1], None, [180, 256], [0, 180, 0, 256])
    cv2.normalize(hist, hist, 0, 255, cv2.NORM_MINMAX)
    return hist

def order_points(pts):
    rect = np.zeros((4, 2), dtype="float32")
    s = pts.sum(axis=1)
    rect[0] = pts[np.argmin(s)]
    rect[2] = pts[np.argmax(s)]
    diff = np.diff(pts, axis=1)
    rect[1] = pts[np.argmin(diff)]
    rect[3] = pts[np.argmax(diff)]
    return rect

# Step 1: Read multiple sample images
sample_images = ['C:/Users/M2-Winterfell/Downloads/Computer-Vision-main/Computer Vision Assignment/Part_II/tables/sample1.jpg',
                 'C:/Users/M2-Winterfell/Downloads/Computer-Vision-main/Computer Vision Assignment/Part_II/tables/sample2.jpg'] 
target_image = cv2.imread('C:/Users/M2-Winterfell/Downloads/Computer-Vision-main/Computer Vision Assignment/Part_II/tables/Table1.jpg')
target_image = rescale_image(target_image, 0.15)

# Apply Color Filtering
hsv = cv2.cvtColor(target_image, cv2.COLOR_BGR2HSV)
v_channel = hsv[:, :, 2]
saturation = hsv[:, :, 1]
v_channel = cv2.equalizeHist(v_channel)
clahe = cv2.createCLAHE(clipLimit=3.0, tileGridSize=(1, 1))
for _ in range(5):
    v_channel = clahe.apply(v_channel)
saturation_multiplier = 2.0
saturation = np.where((saturation * saturation_multiplier) > 255, 255, saturation * saturation_multiplier).astype(np.uint8)
hsv[:, :, 2] = v_channel
hsv[:, :, 1] = saturation
lower_blue = np.array([90, 50, 45])
upper_blue = np.array([170, 255, 255])
mask = cv2.inRange(hsv, lower_blue, upper_blue)

# Step 2: Create a combined histogram model and apply back projection
combined_hist = None
for sample_image_path in sample_images:
    sample_image = cv2.imread(sample_image_path)
    sample_image = rescale_image(sample_image, 0.15)
    hist_table = calculate_histogram(sample_image)
    if combined_hist is None:
        combined_hist = hist_table
    else:
        combined_hist += hist_table
cv2.normalize(combined_hist, combined_hist, 0, 255, cv2.NORM_MINMAX)
hsv_target = cv2.cvtColor(target_image, cv2.COLOR_BGR2HSV)
dst = cv2.calcBackProject([hsv_target], [0, 1], combined_hist, [0, 180, 0, 256], 1)

# Combine color filter result with back projection
combined_result = cv2.bitwise_or(dst, mask)

# Step 3: Post-processing and Finding the Largest Quadrilateral
_, thresholded = cv2.threshold(combined_result, 50, 255, cv2.THRESH_BINARY)
kernel = np.ones((4, 4), np.uint8) #table 3,4,5 perform better when 3,3
dilated = cv2.dilate(thresholded, kernel, iterations=5)
contours, _ = cv2.findContours(dilated, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

largest_area = 0
largest_quad = None
for contour in contours:
    epsilon = 0.05 * cv2.arcLength(contour, True)
    approx = cv2.approxPolyDP(contour, epsilon, True)
    if len(approx) == 4:
        area = cv2.contourArea(approx)
        if area > largest_area:
            largest_area = area
            largest_quad = approx

# Apply Perspective Transform to the largest quadrilateral
if largest_quad is not None:
    ordered_corners = order_points(largest_quad.reshape(4, 2))
    width, height = 300, 200  # Example dimensions
    dst = np.array([[0, 0], [width - 1, 0], [width - 1, height - 1], [0, height - 1]], dtype="float32")
    transform_matrix = cv2.getPerspectiveTransform(ordered_corners, dst)
    warped = cv2.warpPerspective(target_image, transform_matrix, (width, height))
    cv2.imshow("Warped Image", warped)
    cv2.waitKey(0)

# Draw the largest quadrilateral on the target image
if largest_quad is not None:
    cv2.drawContours(target_image, [largest_quad], -1, (0, 255, 0), 3)

# Display the result with detected largest table
cv2.imshow('Detected Largest Table', target_image)
cv2.waitKey(0)
cv2.destroyAllWindows()
