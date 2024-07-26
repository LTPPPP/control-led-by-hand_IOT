import cv2
import mediapipe as mp
import asyncio
import websockets
import base64
import requests

# Initialize the Mediapipe Hands object to track two hands
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=2, min_detection_confidence=0.7, min_tracking_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

ESP8266_IP = "http://192.168.128.175/fingerstatus"  # Replace with the IP address of ESP8266

def is_finger_extended(landmark_data, finger_points):
    """Check if a specific finger is extended."""
    return landmark_data[finger_points[0]].y < landmark_data[finger_points[1]].y < landmark_data[finger_points[2]].y

def is_thumb_extended(landmark_data):
    """Check if the thumb is extended."""
    return (landmark_data[4].x < landmark_data[3].x < landmark_data[2].x if landmark_data[4].y < landmark_data[3].y else landmark_data[4].x > landmark_data[3].x > landmark_data[2].x)

def is_hand_clenched(landmark_data):
    clenched = True
    if is_thumb_extended(landmark_data):
        clenched = False
    for finger_points in [[8, 7, 6], [12, 11, 10], [16, 15, 14], [20, 19, 18]]:
        if is_finger_extended(landmark_data, finger_points):
            clenched = False
    return clenched

async def send_frames(websocket, path):
    cap = cv2.VideoCapture(0)
    try:
        while True:
            # Read a frame from the webcam
            success, image = cap.read()
            if not success:
                print("Failed to read the frame from the webcam.")
                break

            # Flip the image horizontally to make it look like a mirror
            image = cv2.flip(image, 1)
            image = cv2.resize(image, (0, 0), fx=0.7, fy=0.7)
            # Convert the image from BGR to RGB
            rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

            # Process the image using the Mediapipe Hands object
            results = hands.process(rgb_image)

            if results.multi_hand_landmarks:
                left_hand_clenched = False
                right_hand_landmarks = None

                # Check which hand is left and which is right
                for hand_landmarks, handedness in zip(results.multi_hand_landmarks, results.multi_handedness):
                    if handedness.classification[0].label == 'Left':
                        left_hand_clenched = is_hand_clenched(hand_landmarks.landmark)
                    else:
                        right_hand_landmarks = hand_landmarks

                if left_hand_clenched and right_hand_landmarks:
                    # Draw the right hand landmarks and connections on the image
                    mp_drawing.draw_landmarks(image, right_hand_landmarks, mp_hands.HAND_CONNECTIONS)

                    # Check the state of each finger on the right hand
                    fingers = []
                    if is_thumb_extended(right_hand_landmarks.landmark):
                        fingers.append('1')
                    else:
                        fingers.append('0')

                    for finger_points in [[8, 7, 6], [12, 11, 10], [16, 15, 14], [20, 19, 18]]:
                        if is_finger_extended(right_hand_landmarks.landmark, finger_points):
                            fingers.append('1')
                        else:
                            fingers.append('0')

                    # Convert the finger states to a single string
                    finger_state = ''.join(fingers)
                    print(f"Finger state: {finger_state}")

                    # Send the state to ESP8266
                    try:
                        response = requests.get(ESP8266_IP, params={'state': finger_state})
                        print("ESP8266 Response:", response.text)
                    except requests.exceptions.RequestException as e:
                        print(f"Error sending request to ESP8266: {e}")

            # Encode the image to send it via WebSocket
            _, buffer = cv2.imencode('.jpg', image)
            frame = base64.b64encode(buffer).decode('utf-8')
            await websocket.send(frame)

            # Display the image on the screen
            cv2.imshow("Hand Tracking", image)

            # Check if the user pressed the "q" key to exit the program
            if cv2.waitKey(1) & 0xFF == ord("q"):
                break
    except KeyboardInterrupt:
        print("Program interrupted by user.")
    finally:
        # Release the webcam and close the windows
        cap.release()
        cv2.destroyAllWindows()

# Start the WebSocket server
start_server = websockets.serve(send_frames, "0.0.0.0", 8765)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
