import sys
import os
from math import pi, cos, sin
import json

dir = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(dir, "../../common/mapping/turtle"))
from turtle import *

ledsPerMeter = 60
ledDistance = 1000 / ledsPerMeter #sizes are in mm

scale = 1./5000 #scale to fit in the -1,1 canvas

turtle = Turtle()

def line(addPositionToTrail=True):
    for i in range(ledsPerMeter):
        turtle.move(ledDistance, addPositionToTrail)

def figuur1():
    line()
    turtle.turn(-60)
    line()
    turtle.turn(-60)
    line()
    turtle.turn(60)
    line()
    turtle.turn(-60)
    line()
    turtle.turn(-60)
    line()
    turtle.turn(-60)
    line()
    turtle.turn(-60)
    line(False)
    turtle.turn(-60)
    line()

def figuur2():
    line(False)
    turtle.turn(60)

    line()
    turtle.turn(-60)
    line()
    turtle.turn(-60)
    line()
    turtle.turn(-60)
    line()
    turtle.turn(60)
    line()
    turtle.turn(-60)
    line()
    turtle.turn(-60)
    line()
    turtle.turn(-60)
    line(False)
    turtle.turn(-60)
    line(False)
    turtle.turn(-60)
    line()


def complexFigure(angle):
    turtle.setPosition(0,0)
    turtle.setDirection(angle-90)
    figuur1()
    turtle.setPosition(0,0)
    turtle.setDirection(angle-90)
    figuur2()

complexFigure(0)
complexFigure(-120)
complexFigure(-240)

def writePoints(name, points, scale=1.):
    f.write("PixelMap " + name + " = {\n")
    for point in points:
        f.write("    {.x = " + str(point['x'] * scale) +
                ", .y = " + str(point['y'] * scale) + "},\n")
    f.write("};\n\n")

f = open(os.path.join(dir, "schwungMap.hpp"), "w")
writePoints("schwungMap", turtle.trail, scale)
f.close()

# points = list(map(lambda p: {'x': p['x'] ,'y': p['y'] }, ring1))
# with open(os.path.join(dir, "ophanimRing1Map.json"), "w") as outfile:
#     json.dump(points, outfile)

# points = list(map(lambda p: {'x': p['x'] ,'y': p['y'] }, ring3))
# with open(os.path.join(dir, "ophanimRing3Map.json"), "w") as outfile:
#     json.dump(points, outfile)

#export to image for video mask

import cv2
import numpy as np

width = 500
height = 500

# Make empty black image 
img = np.zeros((height, width, 3), np.uint8)

white = [255,255,255]
radius = 2

def writePointsToImg(points):
    for point in points:
        coord = (
            int((point['x']*scale+1) * width/2),
            int((point['y']*scale+1) * height/2),
        )
        if (coord[0] < 0 or coord[0] >= width):
            continue
        if (coord[1] < 0 or coord[1] >= height):
            continue
        print(coord)
        cv2.circle(img, coord, radius, white, -1)

writePointsToImg(turtle.trail)
# writePointsToImg(ring2)
# writePointsToImg(ring3)

cv2.imwrite(os.path.join(dir,"schwungMap.png"), img)
cv2.imshow('img', img)

cv2.waitKey(0)