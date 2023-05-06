from math import *

points = []

scale = 0.8
numLeds = 96

for i in range(numLeds):
    x = scale * cos(float(i)/numLeds*2*pi)
    y = scale * sin(float(i)/numLeds*2*pi)
    points.append({'x': x*scale, 'y': y*scale})

print('number of leds: ', len(points))

f = open("haloMap.hpp", "w")
f.write("PixelMap haloMap = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y'])+ "},\n")
f.write("};\n")
f.close()