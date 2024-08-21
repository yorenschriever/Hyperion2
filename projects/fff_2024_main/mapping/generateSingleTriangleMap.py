from turtle import *
from math import sqrt, cos, asin
import os

turtle = Turtle()

# turtle begins facing to the right
# turning < 0 is counter clockwise

ledsPerLine = 60
ledDistance = 0.03


def ledLine():
    for i in range(ledsPerLine):
        turtle.move(ledDistance)


def triangle():
    # turtle.turn(-90)
    # turtle.move(ledDistance * ledsPerLine/2, False)
    for i in range(3):
        turtle.turn(120)
        ledLine()  
    # turtle.turn(90)


for triangleIndex in range(16):

    # go to the start of the inner triangle
    turtle.setPosition(0, -0.8)
    turtle.setDirection(-90+30)
    #turtle.move(0.35, False)
    triangle()

dir = os.path.dirname(os.path.realpath(__file__))
f = open(os.path.join(dir,"singleTriangleMap.hpp"), "w")
f.write("PixelMap singleTriangleMap = {\n")
for point in turtle.trail:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y']) + "},\n")
f.write("};\n\n")
f.close()

#export to image for video mask

import cv2
import numpy as np

width = 500
height = 500

# Make empty black image 
img = np.zeros((height, width, 3), np.uint8)

white = [255,255,255]
radius = 3

for point in turtle.trail:
    coord = (
        int((point['x']+1) * width/2),
        int((point['y']+1) * height/2),
    )
    if (coord[0] < 0 or coord[0] >= width):
        continue
    if (coord[1] < 0 or coord[1] >= height):
        continue
    print(coord)
    cv2.circle(img, coord, radius, white, -1)

cv2.imwrite(os.path.join(dir,"singleTriangleMap.png"), img)
cv2.imshow('img', img)

cv2.waitKey(0)