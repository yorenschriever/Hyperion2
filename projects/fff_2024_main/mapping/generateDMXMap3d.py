from turtle3d import *
from math import sqrt, pi
import os

turtle = Turtle3d()

ledsPerMeter = 60
ledDistance = 1000 / ledsPerMeter #sizes are in mm

domeRadius = 4000
scale = 1./domeRadius #scale to fit in the -1,1 canvas

boothFront = -domeRadius
boothBack = boothFront - 2000

##### Ledpar

def drawCircle(amount, radius):
    result = []
    for i in range(amount):
        x=radius * cos(float(i)/amount * 2 * pi)
        y=radius * sin(float(i)/amount * 2 * pi)
        z=0
        result.append({'x': x, 'y': y, 'z': z})
    return result

ledpars = drawCircle(12,5000)

##### Blinders

blinders = [
    {'x': -3200, 'y': boothFront, 'z': 2000},
    {'x': -3000, 'y': boothFront, 'z': 2000},

    {'x': 3000,  'y': boothFront, 'z': 2000},
    {'x': 3200,  'y': boothFront, 'z': 2000},

    {'x': -3200, 'y': boothBack, 'z': 1500},
    {'x': -3000, 'y': boothBack, 'z': 1500},

    {'x': 3000,  'y': boothBack, 'z': 1500},
    {'x': 3200,  'y': boothBack, 'z': 1500},
]

##### Eyes

eyes = []

def drawCircle(amount, radius, xc, yc, zc):
    result = []
    for i in range(amount):
        x=xc + radius * cos(float(i)/amount * 2 * pi)
        y=yc
        z=zc + radius * sin(float(i)/amount * 2 * pi)
        result.append({'x': x, 'y': y, 'z': z})
    return result

eyesHeight = 1500
eyes.extend(drawCircle(6,450, -1000,boothBack,eyesHeight))
eyes.append({'x': -1000, 'y': boothBack, 'z': eyesHeight})
eyes.extend(drawCircle(6,450, 1000,boothBack,eyesHeight))
eyes.append({'x': 1000, 'y': boothBack, 'z': eyesHeight})


dir = os.path.dirname(os.path.realpath(__file__))
f = open(os.path.join(dir,"DMXMap3d.hpp"), "w")

f.write("PixelMap3d ledparMap3d = {\n")
for point in ledpars:
    f.write("    {.x = " + str(point['x'] * scale) + ", .y = " + str(point['z'] * scale) + ", .z = " + str(point['y'] * scale) + "},\n")
f.write("};\n\n")

f.write("PixelMap3d blinderMap3d = {\n")
for point in blinders:
    f.write("    {.x = " + str(point['x'] * scale) + ", .y = " + str(point['z'] * scale) + ", .z = " + str(point['y'] * scale) + "},\n")
f.write("};\n\n")

f.write("PixelMap3d eyesMap3d = {\n")
for point in eyes:
    f.write("    {.x = " + str(point['x'] * scale) + ", .y = " + str(point['z'] * scale) + ", .z = " + str(point['y'] * scale) + "},\n")
f.write("};\n\n")

f.close()
