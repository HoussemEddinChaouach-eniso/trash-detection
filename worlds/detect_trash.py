import torch
from PIL import Image
import time
model = torch.hub.load('ultralytics/yolov5', 'custom', path=r'C:\Users\houssem\yolov5\best.pt')

model.conf = 0.25

def detect_trash():
    try:
        img_path = "camera_frame.jpg"
        img = Image.open(img_path)
        results = model(img)
        df = results.pandas().xyxy[0]
        print(df[['name', 'confidence']])
    except Exception as e:
        print("Error:", e)

if __name__ == "__main__":
    while True:
        detect_trash()
        time.sleep(1)
