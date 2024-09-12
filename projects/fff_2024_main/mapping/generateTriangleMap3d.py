from turtle3d import *
from math import sqrt
import os

turtle = Turtle3d()

ledsPerMeter = 60
ledDistance = 1000 / ledsPerMeter #sizes are in mm

barsPerRing=4*6

domeRadius = 4000
scale = 1./domeRadius #scale to fit in the -1,1 canvas

def drawLedLine():
    for i in range(ledsPerMeter):
        turtle.move(ledDistance)

def drawTriangle(pitch, yaw):
    turtle.setPosition(0,0,0)
    turtle.setRotation(yaw + 90, pitch, 0)
    turtle.move(domeRadius, False)

    # correct center point
    # move to the side along the base of the triangle
    turtle.yaw(-90)
    turtle.move(500, False)
    turtle.yaw(90)
    #move down a little
    turtle.pitch(-90)
    triangleHeight = 0.5 * 1000 * sqrt(3)
    turtle.move(0.5 * triangleHeight, False)
    turtle.pitch(90)

    turtle.yaw(90)
    
    for i in range(3):
        drawLedLine() 
        turtle.pitch(120) 

triangles = [
    [25,  -1*55],
    [40,  -1*60],
    [25,  -1*-25],
    [40,  -1*-20],
    [32,  -1*30],
    [45,  -1*5],

    [25,  -1*100],
    [40,  -1*95],
    [32,  -1*125],
    [5,   -1*180],
    [15,  -1*-55],
    [35,  -1*-50],

    [25,  -1*-75],
    [65,  -1*-85],
    [45,  -1*-120],

    [90, 0]
]

for triangle in triangles:
    drawTriangle(triangle[0],triangle[1])

dir = os.path.dirname(os.path.realpath(__file__))
f = open(os.path.join(dir,"triangleMap3d.hpp"), "w")
f.write("PixelMap3d triangleMap3d = {\n")
for point in turtle.trail:
    f.write("    {.x = " + str(point['x'] * scale) + ", .y = " + str(point['z'] * scale) + ", .z = " + str(point['y'] * scale) + "},\n")
f.write("};\n\n")
f.close()
