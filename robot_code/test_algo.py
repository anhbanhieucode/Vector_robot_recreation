import cv2
from ultralytics import YOLO
import time
import numpy as np
# Load YOLOv8 model (you can use your own trained model here)
model = YOLO('yolov8n.pt')  # or use 'yolov8n.pt', 'yolov8s.pt' for faster inference

# You can change this to your target class, e.g., "hand" if trained or "person" as a proxy
TARGET_CLASS = 'person'

# Map class index to names (use COCO classes)
CLASS_NAMES = model.names

cap = cv2.VideoCapture(0)
prev_height = None
al_prev_h = None
set_of_dis = []

print("📹 Starting webcam. Move your hand closer or farther from the camera.")

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    results = model(frame, verbose=False)[0]
    # Find target object
    found = False
    for box in results.boxes:
        cls = int(box.cls[0])
        name = CLASS_NAMES[cls]
        if name == TARGET_CLASS:
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            bbox_height = y2 - y1
            al_bb_h = x2 - x1
            center_x, center_y = (x1 + x2) // 2, (y1 + y2) // 2
            
            if prev_height is not None:
                delta = bbox_height-prev_height
                set_of_dis.append(delta*0.5)

                if len(set_of_dis)>30:
                    average_dis = np.mean(set_of_dis)
                    print(f"average distance is: {average_dis:.2f}")
                    average_dis = []
            else:
                print(f"starting...")
            prev_height = bbox_height
            found = True

            # Draw box and movement
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.putText(frame, f"{name}: {bbox_height}px", (x1, y1 - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
            break

    if not found:
        cv2.putText(frame, "Target not found", (10, 40),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)

    cv2.imshow("Distance Estimation", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
