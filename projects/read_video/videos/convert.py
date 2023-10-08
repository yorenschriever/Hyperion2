#!/opt/homebrew/bin/python3
#pip3 install opencv-python
from multiprocessing import Pool, Queue
import cv2
import json
import sys
import os

if (len(sys.argv)==0):
    raise Exception('call this script from command line or drag a video file into it. eg ./convert.py myVideo.mp4')
videoPath = sys.argv[1]
videoFilename = os.path.basename(videoPath)
videoName = os.path.splitext(videoFilename)[0]

dir = os.path.dirname(__file__)

print ("Reading file:", videoPath)

# read video file
cap = cv2.VideoCapture(videoPath)

if (not cap.isOpened()):
    raise Exception('cannot open video file') 

width  = cap.get(cv2.CAP_PROP_FRAME_WIDTH)   # float `width`
height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)  # float `height`
channels = 3 #opencv automatically converts all video formats to BGR
    
# read pixel map and convert coordinates
with open(dir+"/../mapping/map.json", "r") as infile:
    pointsJson = json.load(infile)

def convertCoordinates(c):
    return [
        int((c['x']+1)*width/2), 
        int((c['y']+1)*height/2)
    ]

points = list(map(convertCoordinates, pointsJson))

# process frames
outName = dir+"/"+videoName+".hpp"
print ("Saving to file:", outName)
f = open(outName, "w")
f.write("Animation anim_"+videoName+" = {\n"+str(int(cap.get(cv2.CAP_PROP_FRAME_COUNT)))+",\n"+str(len(points))+",\n"+str(channels)+",\n{\n")

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()
    if not ret: break

    for p in points:
        bgrPixel = frame[p[1],p[0]]
        f.write("    ")
        for c in range(channels):
            f.write(str(bgrPixel[c])+",")
        f.write("\n")

    f.write("\n")

f.write("}\n};\n")

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
