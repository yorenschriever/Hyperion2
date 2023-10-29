from turtle3d import *
from math import sqrt, cos, asin

turtle = Turtle3d()

# turtle begins facing to the right
# turning < 0 is counter clockwise

ledsPerMeter = 60
ledDistance = 1000 / ledsPerMeter #sizes are in mm

barsPerRing=4*8

totalSize = 2000 #total size of the installation, to calculate the scale
scale = 2/totalSize #scale to fit in the -1,1 canvas

def ledLine():
    for i in range(ledsPerMeter):
        turtle.move(ledDistance)

def reverseLedLine():
    turtle.move(ledDistance * ledsPerMeter, False)
    turtle.turnVertical(180)
    for i in range(ledsPerMeter):
        turtle.move(ledDistance)

for i in range(barsPerRing):
    turtle.setPosition(0,0,1000)
    turtle.setDirection(i * 360 / (barsPerRing), 90)
    turtle.move(500,False)
    turtle.turnVertical(45+25)
    if (i%2==0):
        ledLine()
    else:
        reverseLedLine()


for i in range(barsPerRing):
    turtle.setPosition(0,0,-180)
    turtle.setDirection(i * 360 / (barsPerRing), 90)
    turtle.move(500,False)
    turtle.turnVertical(-30)
    if (i%2==0):
        ledLine()
    else:
        reverseLedLine()


print ("chandelier map leds: ", len(turtle.trail))

# f = open("chandelierMap.hpp", "w")
# f.write("PixelMap chandelierMap = {\n")
# for point in turtle.trail:
#     f.write("    {.x = " + str(point['x'] * scale) + ", .y = " + str(point['y']*scale) + "},\n")
# f.write("};\n\n")
# f.close()

# def find_z(pos):
#     r = sqrt(pos['x']**2 + pos['y']**2)
#     estimate1 = cos(asin(r))
#     estimate2 = 1-0.5*r
#     return (estimate1 + estimate2)/2 - 0.25

f = open("chandelierMap3d.hpp", "w")
f.write("PixelMap3d chandelierMap3d = {\n")
for point in turtle.trail:
    f.write("    {.x = " + str(point['x'] * scale) + ", .z = " + str(point['y'] * scale) + ", .y = " + str(point['z'] * scale) + "},\n")
f.write("};\n\n")
f.close()


totalSize = 14000 #total size of the installation, to calculate the scale
scale = 2/totalSize #scale to fit in the -1,1 canvas
height = 3500
yoffset = 3000
zoffset = -4000

f = open("chandelierMap3dCombined.hpp", "w")
f.write("PixelMap3d chandelierMap3dCombined = {\n")
for point in turtle.trail:
    f.write("    {.x = " + str(point['x'] * scale) + ", .z = " + str((point['y']-yoffset) * scale) + ", .y = " + str((point['z']+height+zoffset) * scale) + "},\n")
f.write("};\n\n")
f.write("float chandelierYOffset = " + str(-yoffset * scale) + ";\n")
f.close()