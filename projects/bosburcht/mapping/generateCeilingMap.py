from turtle import *
from math import sqrt, cos, asin

turtle = Turtle()

# turtle begins facing to the right
# turning < 0 is counter clockwise

ledsPerMeter = 60
ledDistance = 1000 / ledsPerMeter #sizes are in mm

parallelDistance = 500
barsPerQuadrant=12

totalSize = 2 * parallelDistance * (barsPerQuadrant+1) #total size of the installation, to calculate the scale
scale = 2/totalSize #scale to fit in the -1,1 canvas

def ledLine(size):
    for i in range(size):
        turtle.move(ledDistance)

for ymirror  in [-1,1]:
    for xmirror in [-1,1]:
        turtle.setPosition((parallelDistance/2 + 1000) * ymirror,parallelDistance/2 * -1*xmirror)
        turtle.setDirection(180 if ymirror == 1 else 0)
        for i in range(barsPerQuadrant):
            direction = (-1 if i%2==0 else 1) * xmirror * ymirror
            turtle.turn(90 * direction)
            turtle.move(parallelDistance)
            turtle.turn(90 * direction)
            ledLine(ledsPerMeter-1)
            

print ("ceiling map leds: ", len(turtle.trail))

f = open("ceilingMap.hpp", "w")
f.write("PixelMap ceilingMap = {\n")
for point in turtle.trail:
    f.write("    {.x = " + str(point['x'] * scale) + ", .y = " + str(point['y']*scale) + "},\n")
f.write("};\n\n")
f.close()



#=================

roomWidth=7500
roomLength=14000
scale = 2/roomLength #scale to fit in the -1,1 canvas
zoffset = -4000 

def transformTo3d(point2d, translate_x, translate_y):
    return {
        'x': translate_x + point2d['x'], 
        'y': translate_y + point2d['y'], 
        'z': 4500
    }

f = open("ceilingMap3dCombined.hpp", "w")
f.write("PixelMap3d ceilingMap3dCombined = {\n")
for point2d in turtle.trail:
    point = transformTo3d(point2d,0,0)
    f.write("    {.x = " + str(point['x'] * scale) + ", .z = " + str(point['y'] * scale) + ", .y = " + str((point['z']+zoffset) * scale) + "},\n")
f.write("};\n\n")
f.close()



# f = open("ceilingMap3dCombined.hpp", "w")
# f.write("PixelMap3d sunMap3d = {\n")
# for point in turtle.trail:
#     f.write("    {.x = " + str(point['x']) + ", .z = " + str(point['y']) + ", .y = " + str(find_z(point)) + "},\n")
# f.write("};\n\n")
# f.close()