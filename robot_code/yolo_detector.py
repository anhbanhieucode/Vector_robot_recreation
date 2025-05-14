import threading
import urllib.request
import numpy as np
import cv2
from ultralytics import YOLO
from config import ESP32_STREAM_URL

class YoloDetector(threading.Thread):
    def __init__(self, url=ESP32_STREAM_URL, model_path="yolov8n.pt"):
        super().__init__()
        self.url = url
        self.model = YOLO(model_path)
        self.running = False
        self.detected_objects = set()
        self.lock = threading.Lock()

    def run(self):
        self.running = True
        byte_buffer = b""
        try:
            stream = urllib.request.urlopen(self.url, timeout=5)
            print("📸 YOLO thread started.")
            while self.running:
                byte_buffer += stream.read(2048)
                start = byte_buffer.find(b'\xff\xd8')
                end = byte_buffer.find(b'\xff\xd9')

                if start != -1 and end != -1:
                    jpg = byte_buffer[start:end+2]
                    byte_buffer = byte_buffer[end+2:]
                    img_array = np.frombuffer(jpg, dtype=np.uint8)
                    if img_array.size == 0:
                        continue

                    frame = cv2.imdecode(img_array, cv2.IMREAD_COLOR)
                    frame = cv2.flip(frame, 0)
                    if frame is None:
                        continue

                    results = self.model(frame, verbose=False)
                    current_detections = set()

                    for result in results:
                        for box in result.boxes:
                            class_id = int(box.cls[0])
                            label = result.names[class_id]
                            x1, y1, x2, y2 = map(int, box.xyxy[0])
                            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                            cv2.putText(frame, label, (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
                            current_detections.add(label)

                    with self.lock:
                        self.detected_objects = current_detections

                    cv2.imshow("ESP32-CAM YOLOv8", frame)
                    if cv2.waitKey(1) & 0xFF == ord('q'):
                        break
        except Exception as e:
            print("❌ YOLO thread error:", e)
        finally:
            cv2.destroyAllWindows()

    def stop(self):
        self.running = False

    def get_detected_objects(self):
        with self.lock:
            return list(self.detected_objects)
