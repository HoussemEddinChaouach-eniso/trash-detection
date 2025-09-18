# Trash Detection Drone with YOLOv5 and Webots

This project demonstrates an **autonomous drone simulation** for trash detection using the **Webots simulation environment** and a **YOLOv5 deep learning model**.  

## Overview
- Drone simulated in Webots.
- Equipped with a **camera** for real-time images.
- Equipped with a **GPS module** to track drone position.
- Custom-trained **YOLOv5 model** detects trash in the camera feed.
- Outputs detection results with **bounding boxes, confidence, and GPS location**.

## Simulation demo
[Click here to watch the demo](media/demo.mp4)

## Key Features
- **Webots Simulation** for realistic drone and environment.
- **YOLOv5 Detection** for trash identification.
- **Real-time Processing** using OpenCV.
- **GPS Integration** to locate trash.

## How to Run
1. Clone the repo:
   ```bash
   git clone https://github.com/HoussemEddinChaouach-eniso/trash-detection.git
   cd trash-detection
2. (Optional) Create a virtual environment to keep dependencies isolated:
  python -m venv venv
  # Activate the virtual environment
  # Windows:
  venv\Scripts\activate
  # macOS/Linux:
  source venv/bin/activate
3.  Install dependencies:
  pip install torch torchvision torchaudio opencv-python pandas ultralytics
4.  Place your trained YOLOv5 model best.pt in the folder specified in the script:  
  C:\Users\desktop\yolov5\best.pt
5.  Run the detection script:
 python detect_trash.py
6.  The OpenCV window will show the drone’s camera feed with bounding boxes for detected trash.
    Press q to quit the window.
