from turtle import *
from math import sqrt, cos, asin
import json

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

points = list(map(lambda p: {'x': p['x'] * scale,'y': p['y'] * scale }, turtle.trail))
with open("ceilingMap.json", "w") as outfile:
    json.dump(points, outfile)


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



# ====== ceiling index map
# maps the indices to get rid of the zigzag order en quadrants
# the order becomes
# 60 --- 0
# 120 -- 61
# 180 -- 121

numLeds = len(turtle.trail)
numLedsPerQuadrant = int(numLeds/4)
mappedIndices=[]

def fillAsc(startIndex):
    mappedIndices.extend(range(startIndex, startIndex+60) )

def fillDesc(startIndex):
    mappedIndices.extend(range(startIndex+59,startIndex-1,-1) )

startAscending = (numLedsPerQuadrant/60)%2==1
# Q1
for i in range(numLedsPerQuadrant,0,-120):
    if startAscending:
        fillAsc(i-60)
        fillDesc(i-120)
    else:
        fillDesc(i-60)
        fillAsc(i-120)
# Q2
for i in range(0,numLedsPerQuadrant,120):
    fillAsc(i     + numLedsPerQuadrant)
    fillDesc(i+60 + numLedsPerQuadrant)
# Q3
for i in range(numLedsPerQuadrant,0,-120):
    if startAscending:
        fillAsc(i-60     + 2*numLedsPerQuadrant)
        fillDesc(i-120   + 2*numLedsPerQuadrant)
    else:
        fillDesc(i-60    + 2*numLedsPerQuadrant)
        fillAsc(i-120    + 2*numLedsPerQuadrant)
# Q4
for i in range(0,numLedsPerQuadrant,120):
    fillAsc(i       + 3*numLedsPerQuadrant)
    fillDesc(i+60   + 3*numLedsPerQuadrant)        



print(mappedIndices)

f = open("ceilingMappedIndices.hpp", "w")
f.write("#pragma once\n")
f.write("#include <stdint.h>\n")
f.write("uint16_t ceilingMappedIndices[] = {\n")
for index in mappedIndices:
    f.write(str(index)+",\n")
f.write("};\n\n")
f.close()