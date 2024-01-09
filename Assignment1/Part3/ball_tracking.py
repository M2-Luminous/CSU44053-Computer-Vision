import cv2
import numpy as np

def main():
    cap = cv2.VideoCapture('C:/Users/M2-Winterfell/Downloads/Computer-Vision-main/Computer Vision Assignment/Part_III/TableTennis.avi')

    if not cap.isOpened():
        print("Error: Cannot open video file.")
        return -1

    lower_bound = np.array([0, 100, 220])
    upper_bound = np.array([20, 255, 255])
    ball_positions = []
    prev_gray = None
    prev_pos = None
    frames_without_ball = 0
    bounce_detection_enabled = True
    bounce_start_frame = -1
    ball_left_hit = False
    ball_right_hit = False

    fps = cap.get(cv2.CAP_PROP_FPS)

    current_frame = 0
    display_message = ""
    y_trend = []  # List to keep track of y-coordinate changes
    frames_since_last_bounce = 5  # Initialize to 5 to allow initial bounce detection

    while cap.read()[0]:
        ret, frame = cap.read()
        current_frame += 1

        frame_height, frame_width = frame.shape[:2]
        middle_x = frame_width / 2

        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(hsv, lower_bound, upper_bound)

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        if prev_gray is not None:
            diff_gray = cv2.absdiff(gray, prev_gray)
            _, diff_gray = cv2.threshold(diff_gray, 10, 255, cv2.THRESH_BINARY)
            mask = cv2.bitwise_and(mask, diff_gray)

        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        ball_positions.clear()

        for contour in contours:
            area = cv2.contourArea(contour)
            if area > 30:
                mu = cv2.moments(contour)
                mc = (mu['m10'] / mu['m00'], mu['m01'] / mu['m00'])
                ball_positions.append(mc)

        if not ball_positions:
            frames_without_ball += 1
            if frames_without_ball > 4 and display_message != "The ball is out of bounds!":
                print(f"Frame {current_frame}: The ball is out of bounds!")
                display_message = "The ball is out of bounds!"
                bounce_detection_enabled = False
                frames_without_ball = 0
                ball_left_hit = False
                ball_right_hit = False
        else:
            frames_without_ball = 0
            current_pos = ball_positions[-1]

            if bounce_detection_enabled and frames_since_last_bounce >= 5:
                # Update y-coordinate trend for bounce detection
                if len(y_trend) >= 4:
                    if y_trend[-1] > y_trend[-2] and current_pos[1] < y_trend[-1]:
                        print(f"Frame {current_frame}: The ball bounced from the table! Position: ({current_pos[0]:.2f}, {current_pos[1]:.2f})")
                        display_message = "The ball bounced from the table!"
                        frames_since_last_bounce = 0  # Reset the bounce frame counter

                y_trend.append(current_pos[1])
                if len(y_trend) > 10:
                    y_trend.pop(0)

            frames_since_last_bounce += 1  # Increment the bounce frame counter

            # Detect hits by left or right player based on ball position
            if prev_pos:
                if current_pos[0] < prev_pos[0] and current_pos[0] > middle_x:
                    if ball_left_hit and display_message != "Ball hit by the right player!":
                        print(f"Frame {current_frame}: Ball hit by the right player!")
                        display_message = "Ball hit by the right player!"
                        bounce_detection_enabled = True
                        ball_left_hit = False
                    else:
                        ball_left_hit = True

                if current_pos[0] > prev_pos[0] and current_pos[0] <= middle_x:
                    if ball_right_hit and display_message != "Ball hit by the left player!":
                        print(f"Frame {current_frame}: Ball hit by the left player!")
                        display_message = "Ball hit by the left player!"
                        bounce_detection_enabled = True
                        ball_right_hit = False
                    else:
                        ball_right_hit = True

            prev_pos = current_pos

        for position in ball_positions:
            cv2.circle(frame, (int(position[0]), int(position[1])), 10, (0, 255, 0), 2)

        cv2.putText(frame, display_message, (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
        cv2.imshow("Table Tennis Video", frame)

        if cv2.waitKey(int(2500 / fps)) & 0xFF == 27:  # Slow down the playback
            break

        prev_gray = gray.copy()

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
