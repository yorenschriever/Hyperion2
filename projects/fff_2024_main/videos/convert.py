#!/opt/homebrew/bin/python3
#pip3 install opencv-python
import cv2
import json
import sys
import os
import struct

if (len(sys.argv)==0):
    raise Exception('call this script from command line or drag a video file into it. eg ./convert.py myVideo.mp4')
videoPath = sys.argv[1]
videoFilename = os.path.basename(videoPath)
videoName = os.path.splitext(videoFilename)[0]
videoName = videoName.replace(" ","_")

dir = os.path.dirname(__file__)

mapPath = sys.argv[2]
outFolder = sys.argv[3]

print ("Reading file:", videoPath)

# read video file
cap = cv2.VideoCapture(videoPath)

if (not cap.isOpened()):
    raise Exception('cannot open video file') 

width  = cap.get(cv2.CAP_PROP_FRAME_WIDTH)   # float `width`
height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)  # float `height`
channels = 3 #opencv automatically converts all video formats to BGR
    
# read pixel map and convert coordinates
with open(dir+mapPath, "r") as infile:
    pointsJson = json.load(infile)

def convertCoordinates(c):
    return [
        int(((c['x']*1)+1)*width/2), 
        int(((c['y']*1)+1)*height/2)
    ]

points = list(map(convertCoordinates, pointsJson))
frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
pixels = len(points)

# ret, frame = cap.read()
# for p in points:
#     cv2.circle(frame, [p[0],p[1]], 2, [255,255,255], 1)
# cv2.imshow('image',frame)
# cv2.waitKey(0)
# exit()

print ("frames=",frames, "pixels=",pixels,"channels=",channels)

# process frames
outName = os.path.join(dir, "processed", outFolder, videoName+".bin") #dir+"/processed/"+videoName+".bin"
print ("Saving to file:", outName)
f = open(outName, "wb")
f.write(struct.pack('>hhB', frames, pixels, channels))

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()
    if not ret: break

    for p in points:
        bgrPixel = frame[p[1],p[0]]
        f.write(struct.pack('BBB', bgrPixel[0],bgrPixel[1],bgrPixel[2]))

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
