from turtle import *
from math import sqrt, cos, asin,pi
from operator import add

# turtle begins facing to the right
# turning < 0 is counter clockwise

ledsPerMeter = 60
ledDistance = 1000 / ledsPerMeter #sizes are in mm

totalSize = 3500 #total size of the installation, to calculate the scale
scale = 2/totalSize #scale to fit in the -1,1 canvas

windowWidth = 860
windowWidthDouble = 1860
windowHeight = 2870

estimates = [
    66,   #boogje  rechts
    172,  #rechts naar beneden
    112,  #horizontaal
    172,  #links naar boven
    66,   #boogje links
    172,  #midden naar beneden
]

corrections = [
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,-70,0,-70,0,-70],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0]
]
fillupSize = 800

def drawDoubleWindow(correction):
    sizes = list( map(add, estimates, correction) )

    # print("alpha",alpha)
    turtle = Turtle()
    turtle.clearTrail()
    turtle.setPosition(0, -windowHeight/2)

    #boogje rechts
    turtle.setDirection(0)
    for i in range(sizes[0]):
        turtle.move(ledDistance * sizes[2]/2 / sizes[0])

    #recht naar beneden
    turtle.setDirection(90)
    for i in range(sizes[1]):
        turtle.move(ledDistance)

    #horizontaal
    turtle.turn(90)
    for i in range(sizes[2]):
        turtle.move(ledDistance)
        
    #links naar boven
    turtle.turn(90)
    for i in range(sizes[3]):
        turtle.move(ledDistance)

    #boogje links
    turtle.turn(90)
    for i in range(sizes[4]):
        turtle.move(ledDistance * sizes[2]/2 / sizes[4])

    #midden naar beneden
    turtle.turn(90)
    for i in range(sizes[5]):
        turtle.move(ledDistance)

    return turtle

turtle = drawDoubleWindow(corrections[0])

print ("total leds:", len(turtle.trail))

f = open("windowMap.hpp", "w")
f.write("PixelMap windowMap = {\n")
for point in turtle.trail:
    f.write("    {.x = " + str(point['x'] * scale) + ", .y = " + str(point['y']*scale) + "},\n")
f.write("};\n\n")
f.close()

#================

roomWidth=7500
roomLength=14000
scale = 2/roomLength #scale to fit in the -1,1 canvas
zoffset = -4000 #
windowBottom=2660+windowHeight/2

def transformTo3d(point2d, translate_x, translate_y):
    return {
        'x': translate_x, 
        'y': -1*point2d['x'] + translate_y, 
        'z': -1*point2d['y']+windowBottom
    }

f = open("windowMap3dCombined.hpp", "w")
f.write("PixelMap3d windowMap3dCombined = {\n")
c=0
for x in [-3375,3375]:
    for y in range(-7000,7000,1750*2):
        turtle = drawDoubleWindow(corrections[c])
        print("length", str(c), " = ", len(turtle.trail))
        c=c+1
        for point2d in turtle.trail:
            point = transformTo3d(point2d,x,y+windowWidthDouble)
            f.write("    {.x = " + str(point['x'] * scale) + ", .z = " + str(point['y'] * scale) + ", .y = " + str((point['z']+zoffset) * scale) + "},\n")
        for filler in range(len(turtle.trail), fillupSize):
            f.write("    {.x = -2, .z = -2, .y = -2},\n")

f.write("};\n\n")

f.close()