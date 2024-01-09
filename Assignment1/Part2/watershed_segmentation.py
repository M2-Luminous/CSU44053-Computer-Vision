import numpy as np
import cv2
# press f to draw foreground priori
# press b to draw background priori
# press esc to process watershed algorithm
# Function to rescale image
def rescale_image(image, scale):
    width = int(image.shape[1] * scale)
    height = int(image.shape[0] * scale)
    dimensions = (width, height)
    return cv2.resize(image, dimensions, interpolation=cv2.INTER_AREA)

# Global variables
drawing = False
current_label = 'foreground'  # 'foreground' or 'background'
foreground_curves = []  # Store the foreground curves
background_curves = []  # Store the background curves

# Mouse callback function
def draw_curve(event, x, y, flags, param):
    global drawing, foreground_curves, background_curves, current_label

    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        if current_label == 'foreground':
            foreground_curves.append([(x, y)])
        else:
            background_curves.append([(x, y)])

    elif event == cv2.EVENT_MOUSEMOVE and drawing:
        if current_label == 'foreground':
            cv2.line(img, foreground_curves[-1][-1], (x, y), (0, 255, 0), 2)
            foreground_curves[-1].append((x, y))
        else:
            cv2.line(img, background_curves[-1][-1], (x, y), (255, 0, 0), 2)
            background_curves[-1].append((x, y))

    elif event == cv2.EVENT_LBUTTONUP:
        drawing = False

# Load an image
img = cv2.imread('C:/Users/M2-Winterfell/Downloads/Computer-Vision-main/Computer Vision Assignment/Part_II/tables/Table5.jpg')
img = rescale_image(img, scale=0.15)
cv2.namedWindow('image')
cv2.setMouseCallback('image', draw_curve)

while True:
    cv2.imshow('image', img)
    k = cv2.waitKey(1) & 0xFF
    if k == ord('b'):
        current_label = 'background'
    elif k == ord('f'):
        current_label = 'foreground'
    elif k == 27:  # Press ESC to exit
        break

cv2.destroyAllWindows()

# Convert to grayscale and apply a binary threshold
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
ret, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)

# Initialize markers for watershed
markers = np.zeros_like(gray, dtype=np.int32)

# Assign labels to foreground and background curves
for i, curve in enumerate(foreground_curves, start=2):
    for point in curve:
        cv2.circle(markers, point, 5, i, -1)

for i, curve in enumerate(background_curves, start=2+len(foreground_curves)):
    for point in curve:
        cv2.circle(markers, point, 5, i, -1)

# Apply the watershed algorithm
cv2.watershed(img, markers)

# Generate boundaries with a thicker and lighter color
boundary_color = (128, 128, 255)  # Light red color
thickness = 3  # Increased thickness

for y in range(img.shape[0]):
    for x in range(img.shape[1]):
        if markers[y, x] == -1:
            cv2.circle(img, (x, y), 1, boundary_color, thickness)

# Displaying the result
cv2.imshow('Segmented Image', img)
cv2.waitKey(0)
cv2.destroyAllWindows()