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

for a in range(6):
    turtle.setPosition(0,0,2000)
    turtle.setRotation(a * 360 / 6,0, 0)

    line() #radial
    line() #radial
    turtle.yaw(120)
    line() #horizontal
    line() #horizontal
    turtle.yaw(-60)
    turtle.pitch(-30)
    line() #45 degree
    turtle.pitch(-30)
    line() #45 degree
    turtle.pitch(-30)
    turtle.yaw(90)
    turtle.pitch(-30)
    turtle.move(183,False)
    line() #horizontal
    line() #horizontal
    line() #horizontal
    turtle.move(183,False)
    turtle.yaw(-90) 
    line()
    line()

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