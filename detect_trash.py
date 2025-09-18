import cv2
import os
import time
import warnings
from glob import glob
import torch

# Suppress FutureWarning about torch.amp.autocast
warnings.filterwarnings("ignore", category=FutureWarning)

# Load the model with updated AMP settings
def load_model():
    model = torch.hub.load('ultralytics/yolov5', 'custom', path=r'C:\Users\houssem\yolov5\best.pt')
    model.conf = 0.25  # Confidence threshold
    
    # Update AMP settings if using CUDA
    if torch.cuda.is_available():
        from torch.cuda.amp import autocast
        model.amp = True
        model.autocast = lambda: autocast(enabled=True, device_type='cuda')
    
    return model

model = load_model()

def get_latest_image(folder):
    """Get the newest image file with retries"""
    while True:
        try:
            files = glob(os.path.join(folder, "drone_capture_*.png"))
            if not files:
                time.sleep(0.1)
                continue
            newest = max(files, key=os.path.getctime)
            try:
                with open(newest, 'rb') as f:
                    if f.read(1):  # Check if file is readable
                        return newest
            except (IOError, PermissionError):
                time.sleep(0.1)
                continue
        except Exception as e:
            print(f"Error finding image: {e}")
            time.sleep(1)

def detect_trash(img_path):
    """Detect trash and return annotated image with printed locations"""
    for _ in range(5):  # Retry if file is busy
        try:
            img = cv2.imread(img_path)
            if img is None:
                continue
                
            # Perform detection with AMP if available
            with torch.no_grad():
                if torch.cuda.is_available() and model.amp:
                    with model.autocast():
                        results = model(img)
                else:
                    results = model(img)
            
            # Process detections
            detections = results.pandas().xyxy[0]
            
            if not detections.empty:
                print(f"\nDetections in {os.path.basename(img_path)}:")
                print("Class     Confidence  Bounding Box")
                print("----------------------------------")
                
                for _, det in detections.iterrows():
                    label = f"{det['name']}: {det['confidence']:.2f}"
                    coords = f"({int(det['xmin'])},{int(det['ymin'])})-({int(det['xmax'])},{int(det['ymax'])})"
                    print(f"{det['name']:<8} {det['confidence']:.2f}     {coords}")
                    
                    # Draw bounding box
                    cv2.rectangle(img, (int(det['xmin']), int(det['ymin'])), 
                                (int(det['xmax']), int(det['ymax'])), (0, 255, 0), 2)
                    cv2.putText(img, label, (int(det['xmin']), int(det['ymin'])-10), 
                               cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,0), 2)
            
            return img
            
        except Exception as e:
            print(f"Detection error: {e}")
            time.sleep(0.1)
    return None

if __name__ == "__main__":
    image_folder = r"C:\Users\houssem\Desktop"
    cv2.namedWindow("Trash Detection", cv2.WINDOW_NORMAL)
    
    try:
        last_processed = None
        while True:
            img_path = get_latest_image(image_folder)
            
            if img_path and img_path != last_processed:
                result_img = detect_trash(img_path)
                if result_img is not None:
                    cv2.imshow("Trash Detection", result_img)
                    last_processed = img_path
                
            if cv2.waitKey(100) == ord('q'):
                break
                
            time.sleep(0.05)
            
    finally:
        cv2.destroyAllWindows()