import sys
import os
from math import pi, cos, sin
import json

dir = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(dir, "../../common/mapping/turtle"))
from turtle3d import *

ledsPerMeter = 60
ledDistance = 1000 / ledsPerMeter #sizes are in mm

scale = 1./4100 #scale to fit in the -1,1 canvas

turtle = Turtle3d()

def line(addPositionToTrail=True):
    for i in range(ledsPerMeter):
        turtle.move(ledDistance, addPositionToTrail)

for a in range(8):
    turtle.setPosition(0,0,2000)
    turtle.setRotation(a * 360 / 8 + 22.5,0, 0)

    line()
    line()
    turtle.pitch(-30)
    line()
    turtle.pitch(-30)
    line()
    turtle.pitch(-30)
    line()
    line()

turtle.setPosition(0,0,2000)
turtle.setRotation(22.5,0, 0)
turtle.move(2000,False) 
turtle.yaw(90+22.5)
for a in range(8):
    turtle.move(530/2, False)  # Move to the center of the cage
    line()
    turtle.move(530/2, False)
    turtle.yaw(45)
 
turtle.setPosition(0,0,2000-1366)
turtle.setRotation(22.5,0, 0)
turtle.move(3366,False) 
turtle.yaw(90+22.5)
for a in range(8):
    turtle.move(576/2, False)  # Move to the center of the cage
    line()
    line()
    turtle.move(576/2, False)
    turtle.yaw(45)

def writePoints(name, points, scale=1.):
    f.write("PixelMap3d " + name + " = {\n")
    for point in points:
        f.write("    {.x = " + str(point['x'] * scale) +
                ", .y = " + str(point['z'] * scale) +
                ", .z = " + str(point['y'] * scale) + "},\n")
    f.write("};\n\n")

f = open(os.path.join(dir, "cageMap.hpp"), "w")
writePoints("cageMap", turtle.trail, scale)
f.close()