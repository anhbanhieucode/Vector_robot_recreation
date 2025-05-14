import cv2
import torch
import numpy as np
from PIL import Image

# Tải mô hình MiDaS
midas = torch.hub.load("intel-isl/MiDaS", "DPT_Large")
midas.eval()

# Tải hàm tiền xử lý từ MiDaS
midas_transforms = torch.hub.load("intel-isl/MiDaS", "transforms").dpt_transform

# Chọn thiết bị (GPU nếu có, nếu không thì dùng CPU)
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
midas.to(device)

# Mở webcam
cap = cv2.VideoCapture(0)  # Nếu bạn dùng camera USB phụ, có thể cần đổi thành 1 hoặc 2

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        print("Không đọc được khung hình từ webcam.")
        break

    # Tiền xử lý ảnh
    input_image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    input_image = input_image / 255.0  # Normalize to [0, 1]
    input_pil = Image.fromarray((input_image * 255).astype(np.uint8))

    # Chuyển ảnh PIL thành tensor và áp dụng transform từ MiDaS
    input_transformed = midas_transforms(input_pil).unsqueeze(0).to(device)

    # Dự đoán bản đồ độ sâu
    with torch.no_grad():
        prediction = midas(input_transformed)
        prediction = torch.nn.functional.interpolate(
            prediction.unsqueeze(1),
            size=frame.shape[:2],
            mode="bicubic",
            align_corners=False,
        ).squeeze()

    # Chuyển sang numpy và hiển thị
    depth_map = prediction.cpu().numpy()
    depth_normalized = cv2.normalize(depth_map, None, 0, 255, cv2.NORM_MINMAX)
    depth_colormap = cv2.applyColorMap(depth_normalized.astype(np.uint8), cv2.COLORMAP_MAGMA)

    # Hiển thị webcam và depth map
    cv2.imshow("Webcam", frame)
    cv2.imshow("Depth Map", depth_colormap)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()