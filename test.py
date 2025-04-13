from ultralytics import YOLO

# Load a pretrained YOLO11n model
model = YOLO("yolo11n.pt")

# Train the model on the COCO8 dataset for 100 epochs
train_results = model.train(
    data="./wider_face_dataset/data.yaml",
    epochs=5   ,
    imgsz=640,
    device=0,
    batch=8,  # Start small, adjust up if stable
)

# Evaluate the model's performance on the validation set
# metrics = model.val()

# Export the model to ONNX format for deployment
path = model.export(format="onnx")  # Returns the path to the exported model