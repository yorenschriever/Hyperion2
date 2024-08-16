from turtle3d import *
from math import sqrt

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
    [0, 360./7 * 0],
    [0, 360./7 * 1],
    [0, 360./7 * 2],
    [0, 360./7 * 3],
    [0, 360./7 * 4],
    [0, 360./7 * 5],
    [0, 360./7 * 6],

    [30, 360/5 * 0],
    [30, 360/5 * 1],
    [30, 360/5 * 2],
    [30, 360/5 * 3],
    [30, 360/5 * 4],

    [60, 360/3 * 0],
    [60, 360/3 * 1],
    [60, 360/3 * 2],

    [90, 0]
]

for triangle in triangles:
    drawTriangle(triangle[0],triangle[1])

f = open("triangleMap3d.hpp", "w")
f.write("PixelMap3d triangleMap3d = {\n")
for point in turtle.trail:
    f.write("    {.x = " + str(point['x'] * scale) + ", .y = " + str(point['z'] * scale) + ", .z = " + str(point['y'] * scale) + "},\n")
f.write("};\n\n")
f.close()
