from turtle import *
from turtle3d import *
from math import sqrt, cos, asin

# turtle begins facing to the right
# turning < 0 is counter clockwise

ledsPerMeter = 60
ledDistance = 1000 / ledsPerMeter #sizes are in mm

totalSize = 3500 #total size of the installation, to calculate the scale
scale = 2/totalSize #scale to fit in the -1,1 canvas

windowWidth = 860
windowHeight = 2870

def drawWindow(turtle, turn, flip):
    vert1=0
    vert2=0
    hor=0
    for i in range(int(1.5 * ledsPerMeter)):
        turtle.move(ledDistance)
        vert1=vert1+1
    for i in range(1 * ledsPerMeter):
        turtle.move(ledDistance)
        turn(0.42)
        vert1=vert1+1
    for i in range(int(0.5 * ledsPerMeter)):
        turtle.move(ledDistance)
        vert1=vert1+1
    flip()
    for i in range(int(0.5 * ledsPerMeter)):
        turtle.move(ledDistance)
        vert2=vert2+1
    for i in range(1 * ledsPerMeter):
        turtle.move(ledDistance)
        turn(0.42)
        vert2=vert2+1
    for i in range(int(1.5 * ledsPerMeter)):
        turtle.move(ledDistance)
        vert2=vert2+1   
    turn(90)
    for i in range(int(windowWidth/ledDistance)):
        turtle.move(ledDistance) 
        hor=hor+1

    print ("leds in vertical part 1:", vert1)
    print ("leds in vertical part 2:", vert2)
    print ("leds in horizontal part:", hor)
    print ("total leds:", vert1+vert2+hor)

turtle = Turtle()
turtle.setPosition(-windowWidth/2, windowHeight/2)
turtle.setDirection(-90)
def flip():
    turtle.direction = turtle.direction * -1
drawWindow(turtle, turtle.turn, flip)

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
distanceBetweenWindows = 1000

# turtle3d = Turtle3d()

# turtle3d.setPosition(-roomWidth/2, -roomLength/2 + windowWidth/2, zoffset)
# turtle3d.setDirection(-90, 90)
# for i in range(1):
#     def turn3d(angle):
#         turtle3d.turnVertical(angle)
#     def flip3d():
#         turtle3d.direction_ver = turtle3d.direction_ver * -1
#     drawWindow(turtle3d, turn3d, flip3d)
#     turtle3d.turnVertical(-90)
#     turtle3d.move(distanceBetweenWindows + windowWidth, False)

def transformTo3d(point2d, translate_x, translate_y):
    return {
        'x': translate_x, 
        'y': point2d['x'] + translate_y, 
        'z': -1*point2d['y']+windowBottom
    }



f = open("windowMap3dCombined.hpp", "w")

# for x in [-3375,3375]:
#     f.write("PixelMap3d windowMap3dCombined" + ("Left" if x<0 else "Right") +  " = {\n")
#     for y in range(-7000,7000,1750*2):
#         for y2 in [0, 1000]:
#             for point2d in turtle.trail:
#                 point = transformTo3d(point2d,x,y+y2+windowWidth+500)
#                 f.write("    {.x = " + str(point['x'] * scale) + ", .z = " + str(point['y'] * scale) + ", .y = " + str((point['z']+zoffset) * scale) + "},\n")
#     f.write("};\n\n")

f.write("PixelMap3d windowMap3dCombined = {\n")
for x in [-3375,3375]:
    
    for y in range(-7000,7000,1750*2):
        for y2 in [0, 1000]:
            for point2d in turtle.trail:
                point = transformTo3d(point2d,x,y+y2+windowWidth+500)
                f.write("    {.x = " + str(point['x'] * scale) + ", .z = " + str(point['y'] * scale) + ", .y = " + str((point['z']+zoffset) * scale) + "},\n")
f.write("};\n\n")

f.close()