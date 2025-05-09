**SSD Mobilenet V1 demo for SAMA5D2-SOM1-EK1**
Please ensure that you use the following configuration to build this project:
XC32 Compier - v4.60
SAMA5D2_DFP - 1.11.228
CMSIS - 5.9.0

For the Harmon SW versions, please use the below in case you want to re-gen MCC project graph:
    - {name: "csp", version: "v3.20.0"}
    - {name: "core", version: "v3.14.0"}
    - {name: "CMSIS_5", version: "5.9.0"}
    - {name: "usb", version: "v3.15.0"}
    - {name: "bsp", version: "v3.21.1"}


Instructions to run the example:
1. Copy any of the images in the images folder - https://github.com/swapna-gg1/Mantra_ObjReco/tree/master/images to a USB Flash and rename it to image.bmp. Use only 1 image at a time.
2. Run the demo and wait for the following message on the console
   Object Recognition demo with SSD Mobilenet V1 model trained with COCO dataset.
   Please make sure you use image of 300x300 resolution.

3. Insert the USB stick. This will copy the bmp image to a 300x300x3 frame buffer and run the tflite inference model.

