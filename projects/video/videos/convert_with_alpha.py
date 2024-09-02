#!/opt/homebrew/bin/python3
#pip3 install opencv-python

import cv2
import json
import sys
import os
import struct

if (len(sys.argv)!=3):
    raise Exception('call convert_with_alpha.sh to start conversion. eg ./convert_with_alpha.sh myVideo.mp4')
videoPathAlpha = sys.argv[2]
videoPath = sys.argv[1]
videoFilename = os.path.basename(videoPath)
videoName = os.path.splitext(videoFilename)[0]
videoName = videoName.replace(" ","_")

dir = os.path.dirname(__file__)

mapPath = sys.argv[2]

print ("Reading files:", videoPath, videoPathAlpha)

# read video file
cap = cv2.VideoCapture(videoPath)
capA = cv2.VideoCapture(videoPathAlpha)

if (not cap.isOpened()):
    raise Exception('cannot open video file') 

width  = cap.get(cv2.CAP_PROP_FRAME_WIDTH)   # float `width`
height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)  # float `height`
channels = 4
    
# read pixel map and convert coordinates
with open(dir+mapPath, "r") as infile:
    pointsJson = json.load(infile)

def convertCoordinates(c):
    return [
        int((c['x']+1)*width/2), 
        int((c['y']+1)*height/2)
    ]

points = list(map(convertCoordinates, pointsJson))
frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
pixels = len(points)

print ("frames=",frames, "pixels=",pixels,"channels=",channels)

# process frames
outName = dir+"/processed/"+videoName+".bin"
print ("Saving to file:", outName)
f = open(outName, "wb")
f.write(struct.pack('>hhB', frames, pixels, channels))

fnr=0
while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()
    retA, frameA = capA.read()
    if not ret or not retA: break

    fnr=fnr+1
    print (fnr)

    for p in points:
        bgrPixel = frame[p[1],p[0]]
        aPixel = frameA[p[1],p[0]]
        f.write(struct.pack('BBBB', bgrPixel[0],bgrPixel[1],bgrPixel[2],aPixel[0]))

# When everything done, release the capture
cap.release()
capA.release()
cv2.destroyAllWindows()
